/*
 * LICENSE
 *
 * Copyright (c) 2016, David Daniel (dd), david@daniels.li
 *
 * code.cpp is free software copyrighted by David Daniel.
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
#include    <cqlite/code.hpp>

#include    <sqlite3.h>

namespace cqlite {

    /**
     * Whether the given sqlite return value represents a successful value.
     * @param code the return value from the questioned sqlite3 function call
     * @return true iff the given return value corresponds to a function call that
     * succeeded
     */
    bool Code::isSuccess (int code)
    {
        bool success = false;

        switch (code) {
            case SQLITE_OK:
            case SQLITE_ROW:
            case SQLITE_DONE:
                success = true;
                break;
        }

        return success;
    }
}

