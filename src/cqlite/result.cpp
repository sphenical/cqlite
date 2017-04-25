/*
 * LICENSE
 *
 * Copyright (c) 2016, David Daniel (dd), david@daniels.li
 *
 * result.cpp is free software copyrighted by David Daniel.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This program comes with ABSOLUTELY NO WARRANTY.
 * This is free software, and you are welcome to redistribute it
 * under certain conditions.
 */
#include    <cqlite/result.hpp>
#include    <cqlite/error.hpp>
#include    <cqlite/code.hpp>

#include    <sqlite3.h>

#include    <thread>
#include    <chrono>

namespace cqlite {

    /**
     * Creates a new result from the given sqlite3 statement.
     * The given statement is not managed in any way, it must be assured
     * that it is valid as long as this result is used.
     * @param stmt the statement
     * @throws Error if stmt is null
     */
    Result::Result (sqlite3_stmt* stmt) :
        stmt_ {stmt},
        index_ {0},
        state_ {SQLITE_ROW}
    {
        if (stmt_ == nullptr) {
            throw Error {"No valid statement given."};
        }
    }

    /**
     * Returns the number of columns of data currently available.
     * Sometimes the same as Result::columns and sometimes 0.
     * @return the number of columns of data currently available
     */
    std::size_t Result::dataColumns () const
    {
        std::size_t nrColumns = 0;

        int count = sqlite3_data_count (stmt_);

        if (count > 0) {
            nrColumns = static_cast<std::size_t> (count);
        }

        return nrColumns;
    }

    /**
     * Returns the number of columns in the result set returned by the prepared statement.
     * @return the number of columns in the result set returned by the prepared statement
     */
    std::size_t Result::columns () const
    {
        std::size_t count = 0;

        int scount = sqlite3_column_count (stmt_);

        if (scount > 0) {
            count = static_cast<std::size_t> (scount);
        }

        return count;
    }

    /**
     * Advances the result to the next row if there are more rows.
     * @return this result
     * @throws QueryError if its not possible to advance even though the end of result
     * rows has not yet been reached.
     */
    Result& Result::operator++ ()
    {
        if (*this) {
            int result;
            std::size_t count {0};

            while ((result = sqlite3_step (stmt_)) == SQLITE_BUSY && ++count < 5) {
                std::this_thread::sleep_for (std::chrono::milliseconds {20});
            }

            if (Code::isError (result)) {
                throw QueryError {sqlite3_errstr (result)};
            }

            index_ = 0;
            state_ = result;
        }

        return *this;
    }

    /**
     * Advances this result set and returns a copy of this.
     * Note that logically a call this method does not make sense, the returned result
     * refers to the same statement like this one, they interfere with each other.
     * @return a copy of this result
     */
    Result Result::operator++ (int)
    {
        Result copy {*this};
        ++*this;
        return copy;
    }

    /**
     * Extracts an integer from the next column.
     * @return this result
     */
    Result& Result::operator>> (int& value)
    {
        value = sqlite3_column_int (stmt_, index_++);
        return *this;
    }

    /**
     * Extracts a signed 64-bit integer from the next column.
     * @return this result
     */
    Result& Result::operator>> (std::int64_t& value)
    {
        value = sqlite3_column_int64 (stmt_, index_++);
        return *this;
    }

    /**
     * Extracts a std::size_t from the next column.
     * @return this result
     */
    Result& Result::operator>> (std::size_t& value)
    {
        value = static_cast<std::size_t> (sqlite3_column_int64 (stmt_, index_++));
        return *this;
    }

    /**
     * Extracts a double from the next column.
     * @return this result
     */
    Result& Result::operator>> (double& value)
    {
        value = sqlite3_column_double (stmt_, index_++);
        return *this;
    }

    /**
     * Extracts a string from the next column.
     * @return this result
     */
    Result& Result::operator>> (std::string& value)
    {
        const char* text = reinterpret_cast<const char*> (
                sqlite3_column_text (stmt_, index_));

        std::size_t size = static_cast<std::size_t> (
                sqlite3_column_bytes (stmt_, index_));

        ++index_;

        value = std::string {text, size};

        return *this;
    }

    /**
     * Extracts a blob from the next column.
     * @param data the blob to extract
     * @see Result::operator>> (std::tuple<const void*&, std::size_t&> data)
     * @return this result
     */
    Result& Result::operator>> (std::pair<const void*, std::size_t>& data)
    {
        return *this >> std::tie (data.first, data.second);
    }

    /**
     * Extracts a blob from the next column.
     * The blob is represented as a tuple that contains the data and the size of the blob.
     * E.g.
     * @code

     std::size_t length;
     const void* blobData;

     result >> std::tie (blobData, length);

     @endcode
     * @return this result
     */
    Result& Result::operator>> (std::tuple<const void*&, std::size_t&> data)
    {
        std::get<0> (data) = sqlite3_column_blob (stmt_, index_);

        std::get<1> (data) = static_cast<std::size_t> (
                sqlite3_column_bytes (stmt_, index_));

        ++index_;

        return *this;
    }

    /**
     * Whether more rows are available.
     * @return true if more rows are available
     */
    Result::operator bool () const
    {
        return state_ == SQLITE_ROW;
    }

    /**
     * Returns the type of the next available column within this result set.
     * @return the type of the next available column within this result set
     */
    Result::Type Result::type () const
    {
        Type type = Type::Null;

        switch (sqlite3_column_type (stmt_, index_)) {

            case SQLITE_INTEGER:
                type = Type::Integer;
                break;
            case SQLITE_FLOAT:
                type = Type::Float;
                break;
            case SQLITE_TEXT:
                type = Type::Text;
                break;
            case SQLITE_BLOB:
                type = Type::Blob;
                break;

            default:
            case SQLITE_NULL:
                break;
        }

        return type;
    }
}

