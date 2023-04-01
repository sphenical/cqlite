cmake_minimum_required (VERSION 3.18)
project (cqlite-sources)

if (CQLITE_BUILD_SHARED_LIBS)
    add_library (cqlite SHARED)
    if (WIN32)
        target_compile_definitions (cqlite
            PUBLIC
                CQLITE_WINDLL_WORKAROUND
        )

    endif ()
else ()
    add_library (cqlite STATIC)
    set_property (TARGET cqlite
        PROPERTY COMPILE_FLAGS -DCQLITE_STATIC)
endif ()

target_sources (cqlite
    PRIVATE
        cqlite/code.cpp
        cqlite/database.cpp
        cqlite/error.cpp
        cqlite/result.cpp
        cqlite/statement.cpp
)

set_target_properties (cqlite
    PROPERTIES
        CXX_STANDARD 20
        VERSION ${CQLITE_VERSION}
        SOVERSION ${CQLITE_VERSION_MAJOR}
)

target_link_libraries (cqlite
    PRIVATE
        SQLite::SQLite3
)

target_include_directories (cqlite SYSTEM
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_library (cqlite::cqlite ALIAS cqlite)

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

##############
# Installation
##############
if (NOT CQLITE_DISABLE_INSTALLS)
    install (TARGETS cqlite
        EXPORT cqlite-config
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    export (EXPORT cqlite-config NAMESPACE cqlite::)

    install (EXPORT cqlite-config
        NAMESPACE cqlite::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cqlite
    )

    write_basic_package_version_file (
        ${CMAKE_CURRENT_BINARY_DIR}/cqlite-config-version.cmake
        VERSION ${CQLITE_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    install (FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cqlite-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cqlite
    )

    if (WIN32)
        install (FILES
            ${SQLite3_LIBRARY_DLL_LOCATION}
            DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endif ()

    install (FILES
        ${CQLITE_CONFIG_HEADER_FILE}
        ${CQLITE_EXPORT_HEADER_FILE}
        cqlite/code.hpp
        cqlite/database.hpp
        cqlite/error.hpp
        cqlite/result.hpp
        cqlite/statement.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cqlite
    )

endif ()

