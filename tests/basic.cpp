#include    <cqlite/database.hpp>

#include    <gtest/gtest.h>

using namespace cqlite;

namespace {
    const char* const NAME = "Jane ''Fonda'";
    const std::size_t COUNT = 10;
}

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

TEST (basic, createdb)
{
    ASSERT_NO_THROW (create ());
}

TEST (basic, insert)
{
    ASSERT_NO_THROW (insert ());
}

TEST (basic, select)
{
    Database db = insert ();

    Statement stmt = db.prepare ("SELECT id, name FROM foo");

    std::size_t count {0};
    for (Result result = stmt.execute ();
            result;
            ++result, ++count)
    {
        std::string name;
        int id;

        result >> id >> name;

        ASSERT_EQ (id, count + 1);
        ASSERT_STREQ (name.c_str (), NAME);
    }

    ASSERT_EQ (count, COUNT);
}

