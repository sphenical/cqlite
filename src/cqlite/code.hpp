/*
 * LICENSE
 *
 * Copyright (c) 2016, David Daniel (dd), david@daniels.li
 *
 * code.hpp is free software copyrighted by David Daniel.
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
#ifndef CQLITE_CODE_INC
#define CQLITE_CODE_INC

#include    <cqlite/cqlite_export.hpp>

namespace cqlite {

    /**
     * In order to interpret a sqlite return value.
     */
    class CQLITE_EXPORT Code
    {
        public:
            static bool isSuccess (int);
            static bool isError (int);
    };

    /**
     * Whether the given sqlite return value is an error.
     * @param code the return value from the questioned sqlite3 function call
     * @return true iff the given return value corresponds to a function call that failed
     */
    inline bool Code::isError (int code)
    { return !isSuccess (code); }
}

#endif /* CQLITE_CODE_INC */

