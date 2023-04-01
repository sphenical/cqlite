/*
 * LICENSE
 *
 * Copyright (c) 2017, David Daniel (dd), david@daniels.li
 *
 * statements.cpp is free software copyrighted by David Daniel.
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
#include <cqlite/datetime.hpp>

#include <gtest/gtest.h>

#include <iostream>

using namespace cqlite;

namespace {
    Database& createDatabase (Database& db)
    {
        db << "CREATE TABLE foo ("
              "id INTEGER PRIMARY KEY, "
              "name TEXT, "
              "created_at TEXT NOT NULL DEFAULT (STRFTIME ('%Y-%m-%dT%H:%M:%fZ'))"
              ")";

        return db;
    }

    std::size_t countNames (Database& db)
    {
        std::size_t count;

        db.prepare ("SELECT COUNT (name) FROM foo").execute () >> count;

        return count;
    }
} // namespace

TEST (statement, reset_and_reassign_succeeds_in_inserting_multiple_values)
{
    Database db {":memory:"};
    createDatabase (db);

    Statement first = db.prepare ("INSERT INTO foo (name) VALUES (?1)");

    first << "Peter";
    first.execute ();

    ASSERT_EQ (countNames (db), 1);

    first.reset ();
    first << "Sue";
    first.execute ();

    ASSERT_EQ (countNames (db), 2);

    first.reset ();
    first << "Marc";
    first.execute ();

    ASSERT_EQ (countNames (db), 3);
}

TEST (statement, date_time_can_be_inserted)
{
    Database db {":memory:"};
    db << "CREATE TABLE foo ("
          "id INTEGER PRIMARY KEY, "
          "name TEXT, "
          "created_at INTEGER NOT NULL DEFAULT 0"
          ")";

    const auto now = DateTime::clock::now ();

    Statement statement
        = db.prepare ("INSERT INTO foo (name, created_at) VALUES (?1, ?2)");

    statement << "Peter" << now;
    statement.execute ();

    Statement select = db.prepare ("SELECT name, created_at FROM foo");
    Result result = select.execute ();

    ASSERT_TRUE (result);

    std::string name;
    DateTime extracted;

    result >> name >> extracted;

    ASSERT_EQ (now, extracted);
}

