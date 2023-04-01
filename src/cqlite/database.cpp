/*
 * LICENSE
 *
 * Copyright (c) 2016, David Daniel (dd), david@daniels.li
 *
 * database.cpp is free software copyrighted by David Daniel.
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
#include <cqlite/code.hpp>
#include <cqlite/database.hpp>

#include <sqlite3.h>

#include <chrono>
#include <thread>
#include <utility>

namespace cqlite {

    namespace {
        using Callback = void (*) (void*, int, char const*, char const*, sqlite3_int64);
    }

    /**
     * Opens a database connection on the given file.
     * @param path the path to the sqlite3 database file
     * @param mode the mode to open the database in
     * @sa Mode
     * @throws DbError on failure
     */
    Database::Database (const std::string& path, std::uint8_t mode) :
        db_ {nullptr}, hooks_ {}
    {
        int flags
            = (mode & Mode::Create ? SQLITE_OPEN_CREATE : 0)
            | (mode & Mode::ReadWrite ? SQLITE_OPEN_READWRITE : SQLITE_OPEN_READONLY)
            | (mode & Mode::Shared ? SQLITE_OPEN_SHAREDCACHE : 0)
            | (mode & Mode::Private ? SQLITE_OPEN_PRIVATECACHE : 0)
            | (mode & Mode::Uri ? SQLITE_OPEN_URI : 0)
            | (mode & Mode::Memory ? SQLITE_OPEN_MEMORY : 0)
            | (mode & Mode::NoMutex ? SQLITE_OPEN_NOMUTEX : 0)
            | (mode & Mode::FullMutex ? SQLITE_OPEN_FULLMUTEX : 0);

        int result = sqlite3_open_v2 (path.c_str (), &db_, flags, nullptr);

        if (result != SQLITE_OK) {
            throw DbError {sqlite3_errstr (result)};
        }

        // If assumed that "typeof (sqlite3_int64) is_interchangeable_to typeof
        // (std::int64_t)", the following reinterpret_cast is safe.
        sqlite3_update_hook (
            db_, reinterpret_cast<Callback> (&Database::static_update_hook), this);
    }

    Database::Database () : db_ {nullptr}, hooks_ {} {}

    Database::~Database () { sqlite3_close (db_); }

    Database::Database (Database&& other) :
        db_ {other.db_}, hooks_ {std::move (other.hooks_)}
    {
        other.db_ = nullptr;

        if (db_) {
            // rebind the hook - it still contains a pointer to the other database!
            sqlite3_update_hook (
                db_, reinterpret_cast<Callback> (&Database::static_update_hook), this);
        }
    }

    Database& Database::operator= (Database&& other)
    {
        if (this != &other) {

            sqlite3_close (db_);

            db_ = other.db_;
            other.db_ = nullptr;

            hooks_ = std::move (other.hooks_);

            if (db_) {
                // rebind the hook - it still contains a pointer to the other database!
                sqlite3_update_hook (db_,
                    reinterpret_cast<Callback> (&Database::static_update_hook), this);
            }
        }

        return *this;
    }

    /**
     * Returns a prepared statement that is created from the given sql expression.
     * @param sql the sql expression with optional placeholders (`?1`, `:name` etc.)
     * @return the corresponding statement
     * @throws DbError if the given statement cannot be compiled
     */
    Statement Database::prepare (const std::string& sql)
    {
        sqlite3_stmt* stmt;
        std::size_t count {0};
        int result;
        const int Length = static_cast<int> (sql.size ());
        const char* const Content = sql.c_str ();

        while ((result = sqlite3_prepare_v2 (db_, Content, Length, &stmt, nullptr))
                   == SQLITE_BUSY
               && ++count < 5) {
            std::this_thread::sleep_for (std::chrono::milliseconds {20});
        }

        if (Code::isError (result)) {
            throw DbError {sqlite3_errstr (result)};
        }

        return Statement {stmt};
    }

    /**
     * Executes the given statement directly on the database.
     * @param sql the sql to execute
     * @return this database
     * @throws DbError if the sql cannot be executed
     */
    Database& Database::operator<< (const std::string& sql)
    {
        char* errstr = nullptr;
        std::size_t count {0};
        int result;

        while ((result = sqlite3_exec (db_, sql.c_str (), nullptr, nullptr, &errstr))
                   == SQLITE_BUSY
               && ++count < 5) {
            std::this_thread::sleep_for (std::chrono::milliseconds {20});
        }

        if (Code::isError (result)) {
            std::string errmsg;

            if (errstr != nullptr) {
                errmsg = errstr;
                sqlite3_free (errstr);
            }

            throw DbError {errmsg};
        }

        if (errstr != nullptr) {
            sqlite3_free (errstr);
        }

        return *this;
    }

    /**
     * The static update hook function used with the sqlite3 C-API
     * @param me a pointer to a database
     * @param operation SQLITE_INSERT, SQLITE_DELETE or SQLITE_UPDATE
     * @param db the name of the affected database (constructors argument)
     * @param table the name of the affected table
     * @param rowid the id of the affected row
     */
    void Database::static_update_hook (
        void* me, int operation, char const* db, char const* table, std::int64_t rowid)
    {
        Database* self = static_cast<Database*> (me);

        auto range = self->hooks_.equal_range (table);
        auto atHook = range.first;
        const auto endOfHooks = range.second;

        auto catchall = self->hooks_.equal_range ("*");
        auto atCatchall = catchall.first;
        auto endOfCatchall = catchall.second;

        if (atHook != endOfHooks || atCatchall != endOfCatchall) {
            Operation op = Operation::Delete;

            switch (operation) {
                case SQLITE_INSERT:
                    op = Operation::Insert;
                    break;
                case SQLITE_DELETE:
                    op = Operation::Delete;
                    break;
                case SQLITE_UPDATE:
                    op = Operation::Update;
                    break;
                default:
                    return;
            }

            for (; atHook != endOfHooks; ++atHook) {
                atHook->second (op, db, table, rowid);
            }

            for (; atCatchall != endOfCatchall; ++atCatchall) {
                atCatchall->second (op, db, table, rowid);
            }
        }
    }

    /**
     * Returns the last inserted row id.
     * @return the last inserted row id
     */
    std::int64_t Database::lastInsertId () const
    {
        return static_cast<std::int64_t> (sqlite3_last_insert_rowid (db_));
    }
} // namespace cqlite

