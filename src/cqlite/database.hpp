/*
 * LICENSE
 *
 * Copyright (c) 2016, David Daniel (dd), david@daniels.li
 *
 * database.hpp is free software copyrighted by David Daniel.
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
#ifndef CQLITE_DATABASE_INC
#define CQLITE_DATABASE_INC

#include    <cstdint>
#include    <string>
#include    <functional>
#include    <utility>

#include    <cqlite/error.hpp>
#include    <cqlite/statement.hpp>
#include    <cqlite/cqlite_export.hpp>
#include    <cqlite/cqlite_config.hpp>

struct sqlite3;

namespace cqlite {

    class DbError : public Error
    {
        public:
            using Error::Error;
    };

    /**
     * Represents a connection to a sqlite3 database file.
     */
    class CQLITE_EXPORT Database
    {
        public:
            enum class Operation
            {
                Insert,
                Delete,
                Update
            };

            /**
             * The mode to open the database with.
             * By default the database is opened in read-write mode and it is created if
             * it does not already exist.
             */
            enum Mode
            {
                /** No writing, very good for read-only initialization. */
                ReadOnly                = 0,
                /** Read-write, the default */
                ReadWrite               = 1 << 0,
                /** Create if not exist, default */
                Create                  = 1 << 1,
                /** Use a shared cache, so the connection shares the cache with other
                 * instances that also set this. */
                Shared                  = 1 << 2,
                /** Do not participate in a shared cache */
                Private                 = 1 << 3,
                /** Use URIs */
                Uri                     = 1 << 4,
                /** Use an in-memory database */
                Memory                  = 1 << 5,
                /** Multithreaded mode (one connection per thread) */
                NoMutex                 = 1 << 6,
                /** Serialized mode */
                FullMutex               = 1 << 7,
            };

        /**
         * The callback that is triggered on every modifying database operation.
         * @param op Insert, Delete, or Update
         * @param db the name of the affected database (probably just "main")
         * @param table the name of the affected table
         * @param rowid the rowid of the affected row
         */
        using UpdateHook = std::function<
            void (Operation op, const std::string& db, const std::string& table,
                    std::int64_t rowid)>;

        public:
            Database ();
            Database (const std::string&,
                    std::uint8_t = Mode::ReadWrite | Mode::Create | Mode::NoMutex);
            ~Database ();

            Database (const Database&) = delete;
            Database& operator= (const Database&) = delete;
            Database (Database&&);
            Database& operator= (Database&&);

            Statement prepare (const std::string&);
            Database& operator<< (const std::string&);

            /*!
             * @brief Adds an update hook callback that gets called on every update/insert/delete.
             *
             * Any previously set callback will silently be dropped, resp. overridden.
             *
             * @tparam Hook the type of the hook callback (in non-windows-dll version only)
             * @see UpdateHook
             * @param hook the callback
             * @return this database
             */
#ifdef      CQLITE_WINDLL_WORKAROUND
            Database& setUpdateHook (const UpdateHook& hook);
#else
            template <typename Hook>
                Database& setUpdateHook (Hook&& hook);
#endif

            std::int64_t lastInsertId () const;

        private:
            static void static_update_hook (void*, int, char const*, char const*, std::int64_t);

        private:
            sqlite3* db_;
#ifdef      CQLITE_WINDLL_WORKAROUND
            UpdateHook* hook_;
#else
            UpdateHook hook_;
#endif
    };

#ifndef     CQLITE_WINDLL_WORKAROUND
    template <typename Hook>
        inline Database& Database::setUpdateHook (Hook&& hook)
        {
            hook_ = std::forward<Hook> (hook);
            return *this;
        }
#endif
}

#endif /* CQLITE_DATABASE_INC */

