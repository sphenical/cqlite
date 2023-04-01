/*
 * LICENSE
 *
 * Copyright (c) 2018, David Daniel (dd), david.daniel@sphenic.ch
 *
 * basic.cpp is free software copyrighted by David Daniel.
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

#include <gtest/gtest.h>

using namespace cqlite;

namespace {
    const char* const NAME = "Jane ''Fonda'";
    const std::size_t COUNT = 10;
} // namespace

Database create ()
{
    Database db {":memory:"};

    db << "CREATE TABLE foo (id INTEGER PRIMARY KEY ASC, name TEXT)";

    return db;
}

Database insert ()
{
    Database db = create ();

    db << "BEGIN";

    Statement stmt = db.prepare ("INSERT INTO foo (name) VALUES (:name)");

    for (std::size_t i = 0; i < COUNT; ++i) {
        stmt.reset ();

        stmt << NAME;

        stmt.execute ();
    }

    db << "COMMIT";

    return db;
}

TEST (database, creating_database_and_schema_succeeds) { ASSERT_NO_THROW (create ()); }

TEST (database, inserting_multiple_values_into_one_table_succeeds)
{
    ASSERT_NO_THROW (insert ());
}

TEST (database, inserting_and_retrieving_values_from_one_table_succeeds)
{
    Database db = insert ();

    Statement stmt = db.prepare ("SELECT id, name FROM foo");

    std::size_t count {0};
    for (Result result = stmt.execute (); result; ++result, ++count) {
        std::string name;
        int id;

        result >> id >> name;

        ASSERT_EQ (id, count + 1);
        ASSERT_STREQ (name.c_str (), NAME);
    }

    ASSERT_EQ (count, COUNT);
}

