cmake_minimum_required (VERSION 3.18)
project (cqlite-tests)
enable_testing ()

find_package (GTest REQUIRED)
include (GoogleTest)

add_executable (runtests)
add_executable (maintest main.cpp)

target_sources (runtests
    PRIVATE
        runner.cpp
        basic.cpp
        advanced.cpp
        statements.cpp
        move.cpp
)

target_link_libraries (runtests
    cqlite
    GTest::GTest
)
target_include_directories (runtests
    PRIVATE
        ${CMAKE_BINARY_DIR}/src
)

target_link_libraries (maintest
    cqlite
)
target_include_directories (maintest
    PRIVATE
        ${CMAKE_BINARY_DIR}/src
)

set_target_properties (runtests maintest
    PROPERTIES CXX_STANDARD 11
)

## Wincrap dll stuff
if (WIN32)

    string (REGEX REPLACE "([^\\.]+)\\.lib$" "\\1.dll" GTEST_DLL ${GTEST_LIBRARIES})
    string (REGEX REPLACE "([^\\.]+)\\.lib$" "\\1.dll" SQLITE3_DLL ${SQLITE3_LIBRARIES})

    add_custom_command (TARGET runtests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
            ${GTEST_DLL}
            ${SQLITE3_DLL}
            $<TARGET_FILE:cqlite>
        $<TARGET_FILE_DIR:runtests>)
endif ()

#######
## Test
#######
add_test (NAME alltests
    COMMAND runtests --gtest_output=xml:${CMAKE_BINARY_DIR}/cqlite_testresults.xml
    WORKING_DIRECTORY $<TARGET_FILE_DIR:runtests>)

add_test (NAME maintest
    COMMAND maintest
    WORKING_DIRECTORY $<TARGET_FILE_DIR:maintest>)

#############################################
## Command "check" instead of "test",
## that way its possible to add dependencies
## to the target.
#############################################
add_custom_target (check
    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION>
    DEPENDS runtests maintest)

