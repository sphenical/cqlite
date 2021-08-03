/*
 * LICENSE
 *
 * Copyright (c) 2018, David Daniel (dd), david.daniel@sphenic.ch
 *
 * advanced.cpp is free software copyrighted by David Daniel.
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

#include    <functional>

#include    <cstring>
#include    <cstdint>
#include    <tuple>

using namespace cqlite;

namespace {

    struct IdWatcher
    {
        std::int64_t lastId;

        void hook (std::int64_t);
    };

    inline void IdWatcher::hook (std::int64_t id)
    { lastId = id; }

    Database& createDatabase (Database& db)
    {
        db <<
            "CREATE TABLE foo ("
            "id INTEGER PRIMARY KEY, "
            "name TEXT, "
            "created_at TEXT NOT NULL DEFAULT (STRFTIME ('%Y-%m-%dT%H:%M:%fZ'))"
            ")";

        return db;
    }

    Database& insert (Database& db)
    {
        Statement stmt = db.prepare ("INSERT INTO foo (name) VALUES (?1)");

        for (std::size_t i = 1; i < 11; ++i) {
            std::string name {"Mr. Number "};
            name.append (std::to_string (i));
            stmt.reset ();
            stmt << name;
            stmt.execute ();
        }

        return db;
    }
}

TEST (advanced, hook) {
    Database db {":memory:"};
    IdWatcher watch;

    createDatabase (db);

    using namespace std::placeholders;
    db.addUpdateHook ("foo", std::bind (&IdWatcher::hook, &watch, _4));

    insert (db);

    ASSERT_EQ (watch.lastId, 10);
}

TEST (advanced, blob) {
    Database db {":memory:"};
    db <<
        "CREATE TABLE foo ("
        "id INTEGER PRIMARY KEY, "
        "data BLOB DEFAULT NULL)";

    Statement insert = db.prepare (
            "INSERT INTO foo (data) VALUES (?1)");

    using DataType = std::uint16_t;
    DataType data [] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    };
    const std::size_t dataSize = sizeof data;
    const std::size_t dataCount = dataSize / sizeof data [0];

    insert << std::make_pair (data, dataSize);
    insert.execute ();

    Statement select = db.prepare ("SELECT data FROM foo");
    Result result = select.execute ();

    std::size_t size;
    const void* resultData;

    result >> std::tie (resultData, size);

    ASSERT_EQ (size, dataSize);

    auto resultPtr = reinterpret_cast<const DataType*> (resultData);

    for (std::size_t i = 0; i < dataCount; ++i) {
        ASSERT_EQ (i, resultPtr [i]);
    }
}
