/*
 * LICENSE
 *
 * Copyright (c) 2018, David Daniel (dd), david.daniel@sphenic.ch
 *
 * move.cpp is free software copyrighted by David Daniel.
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
#include    <cqlite/database.hpp>

#include    <gtest/gtest.h>

#include    <memory>
#include    <functional>

struct Count
{
    std::size_t count = 0;

    void up ();
};

void Count::up ()
{ ++count; }

TEST (move, assign)
{
    Count count;
    cqlite::Database other;
    {
        cqlite::Database db {":memory:"};

        db << "CREATE TABLE foo (\n"
              "id INTEGER PRIMARY KEY ASC NOT NULL,\n"
              "name TEXT\n"
              ")";

        db.setUpdateHook (std::bind (&Count::up, &count));

        other = std::move (db);
    }

    other << "INSERT INTO foo (name) VALUES ('martin schulz')";

    ASSERT_EQ (count.count, 1);
}


TEST (move, construct)
{
    using DbPtr = std::unique_ptr<cqlite::Database>;

    Count count;
    DbPtr db {new cqlite::Database {":memory:"}};

    *db << "CREATE TABLE foo (\n"
        "id INTEGER PRIMARY KEY ASC NOT NULL,\n"
        "name TEXT\n"
        ")";

    db->setUpdateHook (std::bind (&Count::up, &count));

    cqlite::Database other {std::move (*db)};

    db.reset (nullptr);

    other << "INSERT INTO foo (name) VALUES ('martin schulz')";

    ASSERT_EQ (count.count, 1);
}
