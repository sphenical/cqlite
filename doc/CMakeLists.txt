project (cqlite-docs)
cmake_minimum_required (VERSION 3.2)

include (FindDoxygen)
if (NOT DOXYGEN_FOUND)
    message (FATAL_ERROR "Cannot find doxygen, this is needed for building the docs.")
endif ()

configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

add_custom_target (doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    COMMENT "Generating API documentation.")

if (NOT CQLITE_DISABLE_INSTALLS)
    install (DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION ${CMAKE_INSTALL_DOCDIR})

endif ()

