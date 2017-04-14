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

    Result::Result (sqlite3_stmt* stmt) :
        stmt_ {stmt},
        index_ {0},
        state_ {SQLITE_ROW}
    {
        if (stmt_ == nullptr) {
            throw Error {"No valid statement given."};
        }
    }

    std::size_t Result::dataColumns () const
    {
        std::size_t nrColumns = 0;

        int count = sqlite3_data_count (stmt_);

        if (count > 0) {
            nrColumns = static_cast<std::size_t> (count);
        }

        return nrColumns;
    }

    std::size_t Result::columns () const
    {
        std::size_t count = 0;

        int scount = sqlite3_column_count (stmt_);

        if (scount > 0) {
            count = static_cast<std::size_t> (scount);
        }

        return count;
    }

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

    Result Result::operator++ (int)
    {
        Result copy {*this};
        ++*this;
        return copy;
    }

    Result& Result::operator>> (int& value)
    {
        value = sqlite3_column_int (stmt_, index_++);
        return *this;
    }

    Result& Result::operator>> (std::int64_t& value)
    {
        value = sqlite3_column_int64 (stmt_, index_++);
        return *this;
    }

    Result& Result::operator>> (std::size_t& value)
    {
        value = static_cast<std::size_t> (sqlite3_column_int64 (stmt_, index_++));
        return *this;
    }

    Result& Result::operator>> (double& value)
    {
        value = sqlite3_column_double (stmt_, index_++);
        return *this;
    }

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

    Result& Result::operator>> (std::pair<const void*, std::size_t>& data)
    {
        data.first = sqlite3_column_blob (stmt_, index_);

        data.second = static_cast<std::size_t> (
                sqlite3_column_bytes (stmt_, index_));

        ++index_;

        return *this;
    }

    Result::operator bool () const
    {
        return state_ == SQLITE_ROW;
    }

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

