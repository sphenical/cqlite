##########
## Testing
##########
enable_testing ()

########
## GTest
########
find_path (GTEST_INCLUDE_DIR NAMES gtest/gtest.h)
find_library (GTEST_LIBRARY NAMES gtest gtest_main)
find_package_handle_standard_args (gtest DEFAULT_MSG
    GTEST_LIBRARY GTEST_INCLUDE_DIR)
mark_as_advanced (GTEST_LIBRARY GTEST_INCLUDE_DIR)
set (GTEST_LIBRARIES ${GTEST_LIBRARY})
set (GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR})

if (NOT ${GTEST_FOUND})
    message (FATAL_ERROR "Cannot find the gtest library.")
endif ()

######################
## Include directories
######################
include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (SYSTEM
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
    ${GTEST_INCLUDE_DIRS})

########
## Tests
########
set (TEST_SOURCES
    runner.cpp
    basic.cpp
    advanced.cpp)

##########
## Targets
##########
add_executable (runtests EXCLUDE_FROM_ALL ${TEST_SOURCES})
add_executable (maintest EXCLUDE_FROM_ALL main.cpp)

target_link_libraries (runtests
    cqlite
    ${GTEST_LIBRARIES})

target_link_libraries (maintest
    cqlite)

set_target_properties (
    runtests maintest
    PROPERTIES CXX_STANDARD 11)

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

