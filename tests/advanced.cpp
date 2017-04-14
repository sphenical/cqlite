#include    <cqlite/database.hpp>
#include    <gtest/gtest.h>

#include    <functional>

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
    db.setUpdateHook (std::bind (&IdWatcher::hook, &watch, _4));

    insert (db);

    ASSERT_EQ (watch.lastId, 10);
}
