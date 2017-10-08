project (cqlite-sources)
cmake_minimum_required (VERSION 3.2)

##########
## Sources
##########
set (CQLITE_SOURCES
    cqlite/code.cpp
    cqlite/database.cpp
    cqlite/result.cpp
    cqlite/statement.cpp)

set (CQLITE_INSTALL_HEADERS
    cqlite/code.hpp
    cqlite/database.hpp
    cqlite/error.hpp
    cqlite/result.hpp
    cqlite/statement.hpp)

#####################
# Include directories
#####################
include_directories (SYSTEM
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${SQLITE3_INCLUDE_DIRS})

#########
# Targets
#########
if (CQLITE_BUILD_SHARED_LIBS)
    add_library (cqlite SHARED ${CQLITE_SOURCES})
    set_property (TARGET cqlite
        PROPERTY SOVERSION ${CQLITE_VERSION_MAJOR})
    if (WIN32)
        set (CQLITE_WINDLL_WORKAROUND TRUE)
    endif ()
else ()
    add_library (cqlite STATIC ${CQLITE_SOURCES})
    set_property (TARGET cqlite
        PROPERTY COMPILE_FLAGS -DCQLITE_STATIC)
endif ()

set_target_properties (cqlite PROPERTIES
    VERSION ${CQLITE_VERSION}
    CXX_STANDARD 11)

target_link_libraries (cqlite
    ${SQLITE3_LIBRARIES})

###################
# Generated Headers
###################
set (CQLITE_EXPORT_HEADER_FILE
    ${CMAKE_CURRENT_BINARY_DIR}/cqlite/cqlite_export.hpp)

generate_export_header (cqlite
    BASE_NAME CQLITE
    EXPORT_MACRO_NAME CQLITE_EXPORT
    EXPORT_FILE_NAME ${CQLITE_EXPORT_HEADER_FILE}
    STATIC_DEFINE CQLITE_STATIC)

set (CQLITE_CONFIG_HEADER_FILE
    ${CMAKE_CURRENT_BINARY_DIR}/cqlite/cqlite_config.hpp)

configure_file (cqlite/cqlite_config.hpp.in ${CQLITE_CONFIG_HEADER_FILE} @ONLY)

list (APPEND CQLITE_INSTALL_HEADERS
    ${CQLITE_EXPORT_HEADER_FILE}
    ${CQLITE_CONFIG_HEADER_FILE})

##############
# Installation
##############
if (NOT CQLITE_DISABLE_INSTALLS)
    install (TARGETS cqlite
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})

    if (WIN32)
        string (REGEX REPLACE "([^\\.]+)\\.lib$" "\\1.dll" SQLITE3_DLL ${SQLITE3_LIBRARIES})

        install (FILES
            ${SQLITE3_DLL}
            ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
            DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endif ()

    install (FILES ${CQLITE_INSTALL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cqlite)
endif ()

