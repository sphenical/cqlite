## cqlite
project (cqlite)
cmake_minimum_required (VERSION 3.2)

set (CXX_STANDARD_REQUIRED ON)

include (FindPackageHandleStandardArgs)
include (GenerateExportHeader)
include (GNUInstallDirs)

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

if (NOT ${SQLITE3_FOUND})
    message (FATAL_ERROR "Cannot find the sqlite3 library.")
endif ()

option (CQLITE_BUILD_SHARED_LIBS "Disable the build of the shared library." ON)
option (CQLITE_BUILD_TESTS "Enable testing." OFF)
option (CQLITE_DISABLE_INSTALLS "Disable all installation targets." OFF)
option (CQLITE_BUILD_DOCUMENTATION "Build the cqlite API documentation" OFF)

set (PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set (PACKAGE_VENDOR "Sphenic Systems")
set (PACKAGE_URL "https://github.com/sphenical/cqlite")
set (PACKAGE_BUGREPORT "info@sphenic.ch")
set (PACKAGE_DESCRIPTION_SUMMARY "A tiny c++ sqlite3 wrapper")
set (PACKAGE_README "README.md")
set (PACKAGE_LICENSE "LICENSE")

set (PACKAGE_VERSION_MAJOR "1")
set (PACKAGE_VERSION_MINOR "2")
set (PACKAGE_VERSION_PATCH "0")
set (PACKAGE_VERSION
    "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH}")

if (NOT ${CQLITE_DISABLE_INSTALLS})
    set (CPACK_PACKAGE_NAME ${PACKAGE_NAME})
    set (CPACK_PACKAGE_VERSION ${PACKAGE_VERSION})
    set (CPACK_PACKAGE_VENDOR ${PACKAGE_VENDOR})
    set (CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PACKAGE_DESCRIPTION_SUMMARY})
    set (CPACK_PACKAGE_VERSION_MAJOR ${PACKAGE_VERSION_MAJOR})
    set (CPACK_PACKAGE_VERSION_MINOR ${PACKAGE_VERSION_MINOR})
    set (CPACK_PACKAGE_VERSION_PATCH ${PACKAGE_VERSION_PATCH})

    set (CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_README})
    set (CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_LICENSE})
    set (CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_README})

    if (WIN32)
        set (CPACK_GENERATOR "NSIS")

        set (CPACK_NSIS_DISPLAY_NAME "${PACKAGE_NAME} ${PACKAGE_VERSION}")
        set (CPACK_NSIS_HELP_LINK ${PACKAGE_URL})
        set (CPACK_NSIS_URL_INFO_ABOUT ${PACKAGE_URL})
        set (CPACK_NSIS_CONTACT ${PACKAGE_BUGREPORT})

        set (CPACK_NSIS_MENU_LINKS
            "${CMAKE_INSTALL_DOCDIR}\\\\${PACKAGE_README}" "${CPACK_NSIS_DISPLAY_NAME} - Readme")

    else ()
        set (CPACK_GENERATOR "STGZ;TGZ")
    endif ()

    set (CPACK_SOURCE_PACKAGE_FILE_NAME "${PACKAGE_NAME}-${PACKAGE_VERSION}-src")
    set (CPACK_SOURCE_GENERATOR "TGZ")

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

endif ()

add_subdirectory (src)

if (${CQLITE_BUILD_TESTS})
    add_subdirectory (tests)
endif ()

if (NOT ${CQLITE_DISABLE_INSTALLS})
    install (FILES
        ${CPACK_RESOURCE_FILE_README}
        ${CPACK_RESOURCE_FILE_LICENSE}
        DESTINATION ${CMAKE_INSTALL_DOCDIR})

    include (CPack)
endif ()

if (${CQLITE_BUILD_DOCUMENTATION})
    add_subdirectory (doc)
endif ()
