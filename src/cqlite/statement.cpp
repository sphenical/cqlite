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
#include    <cqlite/statement.hpp>

#include    <sqlite3.h>

namespace cqlite {

    namespace {

        inline void handleResult (int result)
        {
            if (result != SQLITE_OK) {
                throw StatementError {sqlite3_errstr (result)};
            }
        }
    }

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

    Statement::~Statement ()
    {
        sqlite3_finalize (stmt_);
    }

    Statement& Statement::operator<< (const std::pair<const void*, std::size_t>& blob)
    {
        handleResult (
                sqlite3_bind_blob64 (
                    stmt_, ++index_, blob.first, blob.second, SQLITE_TRANSIENT));

        return *this;
    }

    Statement& Statement::operator<< (double value)
    {
        handleResult (sqlite3_bind_double (stmt_, ++index_, value));

        return *this;
    }

    Statement& Statement::operator<< (int value)
    {
        handleResult (sqlite3_bind_int (stmt_, ++index_, value));

        return *this;
    }

    Statement& Statement::operator<< (std::size_t value)
    {
        handleResult (
                sqlite3_bind_int64 (stmt_, ++index_, static_cast<std::int64_t> (value)));

        return *this;
    }

    Statement& Statement::operator<< (std::int64_t value)
    {
        handleResult (sqlite3_bind_int64 (stmt_, ++index_, value));

        return *this;
    }

    Statement& Statement::operator<< (std::nullptr_t)
    {
        handleResult (sqlite3_bind_null (stmt_, ++index_));

        return *this;
    }

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

    Statement& Statement::reset ()
    {
        sqlite3_reset (stmt_);
        index_ = 0;

        return *this;
    }

    Result Statement::execute ()
    {
        Result result {stmt_};
        ++result;
        return result;
    }
}

