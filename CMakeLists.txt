cmake_minimum_required (VERSION 3.18)
project (cqlite
    VERSION 1.3.2
    DESCRIPTION "C++ SQLite3 wrapper library"
)

set (CXX_STANDARD_REQUIRED ON)

include (FindPackageHandleStandardArgs)
include (GenerateExportHeader)
include (GNUInstallDirs)
include (CMakePackageConfigHelpers)


##########
## SQLite3
##########
find_path (SQLITE3_INCLUDE_DIR NAMES sqlite3.h)
find_library (SQLITE3_LIBRARY NAMES sqlite3)
find_package_handle_standard_args (sqlite3 DEFAULT_MSG
    SQLITE3_LIBRARY SQLITE3_INCLUDE_DIR)
mark_as_advanced (SQLITE3_LIBRARY SQLITE3_INCLUDE_DIR)
set (SQLITE3_INCLUDE_DIRS ${SQLITE3_INCLUDE_DIR})
set (SQLITE3_LIBRARIES ${SQLITE3_LIBRARY})

if (NOT SQLITE3_FOUND)
    message (FATAL_ERROR "Cannot find the sqlite3 library.")
endif ()

option (CQLITE_BUILD_SHARED_LIBS "Disable the build of the shared library." ON)
option (CQLITE_BUILD_TESTS "Enable testing." OFF)
option (CQLITE_DISABLE_INSTALLS "Disable all installation targets." OFF)
option (CQLITE_BUILD_DOCUMENTATION "Build the cqlite API documentation" OFF)

set (CQLITE_NAME "cqlite")
set (CQLITE_VENDOR "Sphenic Systems")
set (CQLITE_URL "https://github.com/sphenical/cqlite")
set (CQLITE_BUGREPORT "info@sphenic.ch")
set (CQLITE_DESCRIPTION_SUMMARY "A tiny c++ sqlite3 wrapper")
set (CQLITE_README "README.md")
set (CQLITE_LICENSE "LICENSE")

set (CQLITE_VERSION_MAJOR "1")
set (CQLITE_VERSION_MINOR "2")
set (CQLITE_VERSION_PATCH "3")
set (CQLITE_VERSION
    "${CQLITE_VERSION_MAJOR}.${CQLITE_VERSION_MINOR}.${CQLITE_VERSION_PATCH}")

## Git version
find_program (GIT_COMMAND git)
if (GIT_COMMAND AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.git")

    execute_process (
        COMMAND ${GIT_COMMAND} describe --tags --abbrev=0
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE CQLITE_GIT_PROJECT_VERSION
        ERROR_VARIABLE CQLITE_GIT_PROJECT_VERSION_ERROR
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if (NOT CQLITE_GIT_PROJECT_VERSION_ERROR)

        string (REPLACE "." ";" _GIT_VERSION_PARTS ${CQLITE_GIT_PROJECT_VERSION})

        list (LENGTH _GIT_VERSION_PARTS _NR_GIT_VERSION_PARTS)
        if (_NR_GIT_VERSION_PARTS GREATER 0)
            math (EXPR _NR_GIT_VERSION_PARTS_TO_ITERATE "${_NR_GIT_VERSION_PARTS} - 1")

            foreach (index RANGE ${_NR_GIT_VERSION_PARTS_TO_ITERATE})
                list (GET _GIT_VERSION_PARTS ${index} item)
                if (index EQUAL 0)
                    set (CQLITE_VERSION_MAJOR ${item})
                elseif (index EQUAL 1)
                    set (CQLITE_VERSION_MINOR ${item})
                elseif (index EQUAL 2)
                    set (CQLITE_VERSION_PATCH ${item})
                endif ()
            endforeach ()

            set (CQLITE_VERSION ${CQLITE_GIT_PROJECT_VERSION})
        endif ()

        execute_process (
            COMMAND ${GIT_COMMAND} log -n 1 --format=%H
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE CQLITE_GIT_COMMIT_ID
            OUTPUT_STRIP_TRAILING_WHITESPACE)

        message (STATUS "cqlite commit id: ${CQLITE_GIT_COMMIT_ID}")

    endif ()

endif ()
message (STATUS "${CQLITE_NAME} version: ${CQLITE_VERSION}")

if (NOT CQLITE_DISABLE_INSTALLS)
    set (CPACK_PACKAGE_NAME ${CQLITE_NAME})
    set (CPACK_PACKAGE_VERSION ${CQLITE_VERSION})
    set (CPACK_PACKAGE_VENDOR ${CQLITE_VENDOR})
    set (CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CQLITE_DESCRIPTION_SUMMARY})
    set (CPACK_PACKAGE_VERSION_MAJOR ${CQLITE_VERSION_MAJOR})
    set (CPACK_PACKAGE_VERSION_MINOR ${CQLITE_VERSION_MINOR})
    set (CPACK_PACKAGE_VERSION_PATCH ${CQLITE_VERSION_PATCH})

    set (CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/${CQLITE_README})
    set (CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/${CQLITE_LICENSE})
    set (CPACK_RESOURCE_FILE_WELCOME ${CMAKE_CURRENT_SOURCE_DIR}/${CQLITE_README})
    set (CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${CQLITE_README})

    if (WIN32)
        set (CPACK_GENERATOR "NSIS")

        set (CPACK_NSIS_DISPLAY_NAME "${CQLITE_NAME} ${CQLITE_VERSION}")
        set (CPACK_NSIS_HELP_LINK ${CQLITE_URL})
        set (CPACK_NSIS_URL_INFO_ABOUT ${CQLITE_URL})
        set (CPACK_NSIS_CONTACT ${CQLITE_BUGREPORT})

        set (CPACK_NSIS_MENU_LINKS
            "${CMAKE_INSTALL_DOCDIR}\\\\${CQLITE_README}" "${CPACK_NSIS_DISPLAY_NAME} - Readme")

    else ()
        set (CPACK_GENERATOR "STGZ;TGZ")
    endif ()

    set (CPACK_SOURCE_PACKAGE_FILE_NAME "${CQLITE_NAME}-${CQLITE_VERSION}-src")
    set (CPACK_SOURCE_IGNORE_FILES
        ".*~$"
        ".git"
        ".gitignore"
        ".*swp$"
        ".*zip$"
        ".*gz$"
        "Makefile.*"
        "^CPack.*"
        "_CPack_Packages"
        "CPackConfig.cmake"
        "CPackSourceConfig.cmake"
        "CMakeFiles"
        "CMakeCache.txt"
        "build.*/"
        ".ycm*"
        "tags")

    set (CPACK_SOURCE_GENERATOR "TGZ")

endif ()

add_subdirectory (src)

if (CQLITE_BUILD_TESTS)
    add_subdirectory (tests)
endif ()

if (CQLITE_BUILD_DOCUMENTATION)
    add_subdirectory (doc)
endif ()

if (NOT CQLITE_DISABLE_INSTALLS)
    install (FILES
        ${CPACK_RESOURCE_FILE_README}
        ${CPACK_RESOURCE_FILE_LICENSE}
        DESTINATION ${CMAKE_INSTALL_DOCDIR})

    include (CPack)
endif ()
