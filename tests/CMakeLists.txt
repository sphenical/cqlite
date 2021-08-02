cmake_minimum_required (VERSION 3.18)
project (cqlite-tests)
enable_testing ()

find_package (GTest REQUIRED)
include (GoogleTest)

#############################################
## Command "check" instead of "test",
## that way its possible to add dependencies
## to the target.
#############################################
add_custom_target (check
    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION>
)

add_executable (cqlite_tests)

target_sources (cqlite_tests
    PRIVATE
        runner.cpp
        basic.cpp
        advanced.cpp
        statements.cpp
        move.cpp
)

target_link_libraries (cqlite_tests
    PRIVATE
        cqlite
        GTest::gtest
        GTest::gtest_main
)

gtest_discover_tests (cqlite_tests
    WORKING_DIRECTORY $<TARGET_FILE_DIR:cqlite_tests>
    XML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/test-results
)

set_target_properties (cqlite_tests
    PROPERTIES CXX_STANDARD 11
)

add_dependencies (check cqlite_tests)

