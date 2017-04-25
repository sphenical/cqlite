/*
 * LICENSE
 *
 * Copyright (c) 2016, David Daniel (dd), david@daniels.li
 *
 * result.hpp is free software copyrighted by David Daniel.
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
#ifndef CQLITE_RESULT_INC
#define CQLITE_RESULT_INC

#include    <cstddef>
#include    <string>
#include    <stdexcept>
#include    <utility>
#include    <tuple>

#include    <cqlite/error.hpp>
#include    <cqlite/cqlite_export.hpp>

struct sqlite3_stmt;

namespace cqlite {

    class QueryError : public Error
    {
        public:
            using Error::Error;
    };

    /**
     * Represents a result from a sql operation.
     */
    class CQLITE_EXPORT Result
    {
        public:

            enum class Type
            {
                Null,
                Integer,
                Float,
                Text,
                Blob
            };

        public:
            Result (sqlite3_stmt*);

            std::size_t columns () const;
            std::size_t dataColumns () const;

            Result& operator++ ();
            Result operator++ (int);

            Result& operator>> (int&);
            Result& operator>> (std::size_t&);
            Result& operator>> (std::int64_t&);
            Result& operator>> (double&);
            Result& operator>> (std::string&);
            Result& operator>> (std::pair<const void*, std::size_t>&);
            Result& operator>> (std::tuple<const void*&, std::size_t&>);

            operator bool () const;
            Type type () const;

        private:
            sqlite3_stmt* stmt_;
            int index_;
            int state_;
    };
}

#endif /* CQLITE_RESULT_INC */

