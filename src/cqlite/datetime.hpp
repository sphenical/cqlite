/*
 * LICENSE
 *
 * Copyright (c) 2022, David Daniel (dd), david.daniel@sphenic.ch
 *
 * datetime.hpp is free software copyrighted by David Daniel.
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
#ifndef CQLITE_DATETIME_INC
#define CQLITE_DATETIME_INC

#include <chrono>

namespace cqlite {
    using DateTime = std::chrono::time_point<std::chrono::system_clock>;
}

#endif /* CQLITE_DATETIME_INC */


