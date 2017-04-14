# cqlite - A very simplistic and incomplete but usable c++ SQLite wrapper

`cqlite` is a tiny c++ wrapper library that encapsulates the C API of
[SQLite](https://www.sqlite.org/). There are a lot of features not exposed from the
`sqlite3` library but for very simple use cases the existing functionality may suffice.

## Usage

`cqlite` mainly encapsulates the following tasks:

- Opening and closing the database.
- Executing SQL.
- Creating, binding and executing prepared statements.
- Extracting basic types (strings, numbers, binaries) from results.

The classes Database, Statement and Result are modeled within the `cqlite` namespace and
their headers are named with their corresponding lowercase name, ending with `.hpp`. The
constructor of a database instance takes the path to the file (or the string ":memory:"
for an in-memory database, just as when using this in the `sqlite3` library).  On this
instance, a string streamed to will be passed to the execute method of `sqlite3` on this
database and a call to prepare returns a prepared statement. The statement can then be
bound with the stream (or shift) operators to values it refers to and or previously be
reset. It then gets executed and if the corresponding statement returns a result, it can
be retrieved, its result values extracted and increased in order to advance in the result
set.

The following example shows a simplified usage:

```c++
#include <cqlite/database.hpp>
#include <string>
#include <iostream>

int main ()
{
    cqlite::Database db {":memory:"};
    db
        << "CREATE TABLE authors  ( "
           "id INTEGER PRIMARY KEY, "
           "name TEXT, "
           "created_at TEXT NOT NULL DEFAULT ("
               "STRFTIME ('%Y-%m-%dT%H:%M:%fZ'))"
           ")";

    cqlite::Statement insert = db.prepare (
        "INSERT INTO authors (name) VALUES (?1)");

    for (std::size_t i = 0; i < 20; ++i) {

        insert.reset ();
        insert << std::string {"Aldous Huxley "}
            .append (std::to_string (i));

        insert.execute ();
    }

    cqlite::Statement select = db.prepare (
        "SELECT id, name, DATETIME (created_at, 'localtime') FROM authors");

    for (cqlite::Result author = select.execute (); author; ++author) {

        std::int64_t id;
        std::string name, created_at;

        author >> id >> name >> created_at;

        std::cout
            << "ID: " << id
            << ", name: " << name
            << ", created at: " << created_at;
    }
}

```

Database and Statement instances cannot be copied but they can be moved. They take care
about resource management but they do not take care about concurrent access.

## Building

The project uses `cmake` as a build tool and allows for different use case scenarios. For
instance it is possible to install the library system wide as a shared library or it is
possible to generate a static library and use this project as a sub-project within a
parent project, for instance in a sub-folder called `thirdparty` or `vendor` or the like.

### As a system wide shared library (default)

Linux:
```sh
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make
$ sudo make install
```

Windows (msys git shell or MS Visual terminal):
```sh
$ mkdir build
$ cd build
$ cmake -G"Visual Studio 14 2015 Win64" ..
$ cmake --build . --config Release --target package
```
The library can then be installed from the generated installer.

### As a sub-project (using the static library build)

In the following it is assumed that the parent project uses a folder called `vendor` to
store additional sub-projects contents that are used as project specific dependencies. It
is further assumed that the `cqlite` library is placed within a correspondingly named
subfolder, possibly populated by the following git command:

```sh
$ git remote add vendor-cqlite https://github.com/sphenical/cqlite
$ git subtree add --prefix vendor/cqlite vendor-cqlite master --squash
```

Within the parent project the sub-project can be built without touching the parent
projects installation rules, producing a static library target that can then be used on
the parent project. This can be achieved with the following content placed in the
top-level CMakeLists.txt file:

```
set (CQLITE_BUILD_SHARED_LIBS OFF CACHE BOOL "Build the cqlite shared library.")
set (CQLITE_DISABLE_INSTALLS ON CACHE BOOL "Install the cqlite library.")
add_subdirectory (vendor/cqlite)
```

This leads to a static library target called `cqlite` that can then be linked to, like
shown in the following example:
```
target_link_libraries (my_target cqlite)
```
The library will then be statically linked to the application without the need to install
any shared object or the like. This is suitable for an application that does not want to
expose the need of the availability of this library on the target host system.

Last but not least the include directories need to be extended in order to be able to
include the `cqlite` header files and the generated header files that are created during
the cmake configuration. This can be achieved by the following call to
`include_directories` that is placed in the `CMakeLists.txt` where the application is
configured:

```
include_directories (SYSTEM
    ${CMAKE_SOURCE_DIR}/vendor/cqlite/src
    ${CMAKE_BINARY_DIR}/vendor/cqlite/src)
```

### For development purposes

If changes are to be made and tests have to be run or extended, the build is instructed to
build the tests. This is done with the build option `-DCQLITE_BUILD_TESTS=ON` that is
passed to the cmake command-line during configuration. Additionally the debug mode is
activated with `-DCMAKE_BUILD_TYPE=Debug`.

#### Pre-requisites

The tests use the [`googletest`](https://github.com/google/googletest) library. If it is
built from sources or is otherwise installed on non-default locations (e.g. on Windows)
its location can be provided by specifying a prefix path for cmake with
`-DCMAKE_PREFIX_PATH='C:\Program Files\googletest-distribution;C:\sqlite3'` for example in
order to specify where the libraries and include files for `googletest` and `sqlite3` can
be found.

##### Windows

On Windows all the libraries used (`sqlite3` and `googletest`) also have to be available
as debug versions. Furthermore they need to be compiled with the same linking model
(static or dynamic (`/MD` vs. `/MT`)), otherwise linker errors will occur.

#### Running the tests

When `CQLITE_BUILD_TESTS=ON` is set, an additional target called `check` is generated that
can then be invoked.

Linux:
```sh
$ make check
```

Windows:
```
$ cmake --build . --config Debug --target check
```
