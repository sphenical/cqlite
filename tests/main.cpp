/*
 * LICENSE
 *
 * Copyright (c) 2017, David Daniel (dd), david@daniels.li
 *
 * main.cpp is free software copyrighted by David Daniel.
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

#include <cqlite/database.hpp>
#include <string>
#include <iostream>

using namespace cqlite;

int main ()
{
    Database db {":memory:"};
    db
        << "CREATE TABLE authors  ( "
        "id INTEGER PRIMARY KEY, "
        "name TEXT, "
        "created_at TEXT NOT NULL DEFAULT ("
        "STRFTIME ('%Y-%m-%dT%H:%M:%fZ'))"
        ")";

    Statement insert = db.prepare (
            "INSERT INTO authors (name) VALUES (?1)");

    for (std::size_t i = 0; i < 20; ++i) {

        insert.reset ();
        insert << std::string {"Aldous Huxley "}
            .append (std::to_string (i));

        insert.execute ();
    }

    Statement select = db.prepare (
            "SELECT id, name, DATETIME (created_at, 'localtime') FROM authors");

    for (Result author = select.execute (); author; ++author) {
        std::int64_t id;
        std::string name, created_at;
        author >> id >> name >> created_at;

        std::cout << "Id: " << id << ", Name: " << name << ", created at: " << created_at << '\n';
    }
}

