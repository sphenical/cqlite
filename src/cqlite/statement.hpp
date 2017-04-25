/*
 * LICENSE
 *
 * Copyright (c) 2016, David Daniel (dd), david@daniels.li
 *
 * statement.hpp is free software copyrighted by David Daniel.
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
#ifndef CQLITE_STATEMENT_INC
#define CQLITE_STATEMENT_INC

#include    <stdexcept>
#include    <cstdint>
#include    <tuple>

#include    <cqlite/error.hpp>
#include    <cqlite/result.hpp>
#include    <cqlite/cqlite_export.hpp>

struct sqlite3_stmt;

namespace cqlite {

    class StatementError : public Error
    {
        public:
            using Error::Error;
    };

    /**
     * An executable statement.
     */
    class CQLITE_EXPORT Statement
    {
        public:
            Statement (sqlite3_stmt*);
            ~Statement ();

            Statement (const Statement&) = delete;
            Statement& operator= (const Statement&) = delete;
            Statement (Statement&&);
            Statement& operator= (Statement&&);

            Statement& operator<< (const std::tuple<const void*, std::size_t>&);
            Statement& operator<< (double);
            Statement& operator<< (int);
            Statement& operator<< (std::size_t);
            Statement& operator<< (std::int64_t);
            Statement& operator<< (std::nullptr_t);
            Statement& operator<< (const std::string&);

            Statement& reset ();

            Result execute ();

        private:
            sqlite3_stmt* stmt_;
            int index_;
    };
}

#endif /* CQLITE_STATEMENT_INC */

