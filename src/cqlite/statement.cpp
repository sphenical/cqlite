/*
 * LICENSE
 *
 * Copyright (c) 2016, David Daniel (dd), david@daniels.li
 *
 * statement.cpp is free software copyrighted by David Daniel.
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
#include <cqlite/datetime.hpp>
#include <cqlite/statement.hpp>

#include <sqlite3.h>

namespace cqlite {

    namespace {

        /**
         * Convenience function to deal with the result of a sqlite3 bind call.
         * @param result the result value of the called sqlite3 bind function
         * @throws StatementError if result is not equal to SQLITE_OK
         */
        inline void handleResult (int result)
        {
            if (result != SQLITE_OK) {
                throw StatementError {sqlite3_errstr (result)};
            }
        }
    }

    /**
     * Constructs a new statement that represents and owns the given sqlite3 statement.
     * @param stmt the corresponding sqlite3 statement
     * @throws StatementError if statement is null
     */
    Statement::Statement (sqlite3_stmt* stmt) :
        stmt_ {stmt},
        index_ {0}
    {
        if (!stmt_) {
            throw StatementError {"No valid statement given"};
        }
    }

    Statement::Statement (Statement&& other) :
        stmt_ {other.stmt_},
        index_ {other.index_}
    {
        other.stmt_ = nullptr;
        other.index_ = 0;
    }

    Statement& Statement::operator= (Statement&& other)
    {
        if (this != &other) {
            if (stmt_) {
                sqlite3_finalize (stmt_);
            }

            stmt_ = other.stmt_;
            index_ = other.index_;

            other.stmt_ = nullptr;
            other.index_ = 0;
        }

        return *this;
    }

    /**
     * Cleans up the underlying sqlite3 statement
     */
    Statement::~Statement ()
    {
        sqlite3_finalize (stmt_);
    }

    /**
     * Binds a blob.
     * @param blob a pair that holds the data and the size of the blob to be bound
     * @return this statement
     * @throws StatementError if the given blob cannot be bound
     */
    Statement& Statement::operator<< (const std::tuple<const void*, std::size_t>& blob)
    {
        handleResult (
                sqlite3_bind_blob64 (
                    stmt_,
                    ++index_,
                    std::get<0> (blob),
                    std::get<1> (blob),
                    SQLITE_TRANSIENT));

        return *this;
    }

    /**
     * Binds a double.
     * @param value the value to bind
     * @return this statement
     * @throws StatementError if the given value cannot be bound
     */
    Statement& Statement::operator<< (double value)
    {
        handleResult (sqlite3_bind_double (stmt_, ++index_, value));

        return *this;
    }

    /**
     * Binds a signed integer.
     * @param value the value to bind
     * @return this statement
     * @throws StatementError if the given value cannot be bound
     */
    Statement& Statement::operator<< (int value)
    {
        handleResult (sqlite3_bind_int (stmt_, ++index_, value));

        return *this;
    }

    /**
     * Binds an std::size_t.
     * @param value the value to bind
     * @return this statement
     * @throws StatementError if the given value cannot be bound
     */
    Statement& Statement::operator<< (std::size_t value)
    {
        handleResult (
                sqlite3_bind_int64 (stmt_, ++index_, static_cast<std::int64_t> (value)));

        return *this;
    }

    /**
     * Binds a 64-bit signed integer.
     * @param value the value to bind
     * @return this statement
     * @throws StatementError if the given value cannot be bound
     */
    Statement& Statement::operator<< (std::int64_t value)
    {
        handleResult (sqlite3_bind_int64 (stmt_, ++index_, value));

        return *this;
    }

    /**
     * Binds null.
     * @return this statement
     * @throws StatementError if null cannot be bound
     */
    Statement& Statement::operator<< (std::nullptr_t)
    {
        handleResult (sqlite3_bind_null (stmt_, ++index_));

        return *this;
    }

    /**
     * Binds a string.
     * @param value the string to bind
     * @return this statement
     * @throws StatementError if the given string cannot be bound
     */
    Statement& Statement::operator<< (const std::string& value)
    {
        handleResult (
                sqlite3_bind_text64 (
                    stmt_,
                    ++index_,
                    value.c_str (),
                    value.size (),
                    SQLITE_TRANSIENT,
                    SQLITE_UTF8));

        return *this;
    }

    /**
     * Binds a std::chrono::time_point<std::chrono::system_clock>
     * @param dateTime the time_point to bind
     * @return this statement
     * @throws StatementError if the given string cannot be bound
     */
    Statement& Statement::operator<< (const DateTime& dateTime)
    {
        handleResult (
                sqlite3_bind_int64 (
                    stmt_,
                    ++index_,
                    dateTime.time_since_epoch ().count ()));

        return *this;
    }

    /**
     * Resets the statement.
     * This resets all parameters previously bound and rewinds the binding to the first
     * parameter. This method comes in in loops, when parameters are bound subsequently
     * with multiple entities. E.g.
     * @code

     std::vector<Thing> things;
     cqlite::Statement statement = db.prepare (
         "INSERT INTO things (name, age, role) VALUES (?1, ?2, ?3)");
     ...
     for (const auto& thing : things) {
        statement.reset ();
        statement << thing.name << thing.age << thing.role;
        statement.execute ();
     }

     @endcode
     @return this statement
     */
    Statement& Statement::reset ()
    {
        sqlite3_reset (stmt_);
        index_ = 0;

        return *this;
    }

    /**
     * Executes the statement and returns the corresponding result.
     * @return the result of the corresponding execution of this statement.
     */
    Result Statement::execute ()
    {
        Result result {stmt_};
        ++result;
        return result;
    }
}

