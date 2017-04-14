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

    class CQLITE_EXPORT Database
    {
        public:
            enum class Operation
            {
                Insert,
                Delete,
                Update
            };

        using UpdateHook = std::function<
            void (Operation, const std::string&, const std::string&, std::int64_t)>;

        public:
            Database (const std::string&);
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
             * The callback has the following signature:
             * void (Operation op, const std::string& db, const std::string& table, std::int64_t row);
             *
             * op: Insert, Delete, or Update
             * db: the name of the affected database
             * table: the name of the affected table
             * row: the rowid of the affected row
             *
             * Any previously set callback will silently be dropped, resp. overridden.
             *
             * @tparam Hook the type of the hook callback (in non-windows-dll version only)
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

