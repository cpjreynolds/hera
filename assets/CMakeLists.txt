file(GLOB_RECURSE ASSETS_INPUT
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS
    *)
list(FILTER ASSETS_INPUT EXCLUDE REGEX ".*CMakeLists\.txt$")
list(FILTER ASSETS_INPUT EXCLUDE REGEX ".*\.DS_Store$")

block(PROPAGATE ASSETS_OUTPUT)
foreach(asset ${ASSETS_INPUT})
    set(infile "${CMAKE_CURRENT_SOURCE_DIR}/${asset}")
    set(outfile "${CMAKE_CURRENT_BINARY_DIR}/${asset}")
    add_custom_command(
        OUTPUT ${outfile}
        COMMAND ${CMAKE_COMMAND} -E copy ${infile} ${outfile}
        MAIN_DEPENDENCY ${infile}
        COMMENT "copying: ${asset}" VERBATIM COMMAND_EXPAND_LISTS)
    list(APPEND ASSETS_OUTPUT "${outfile}")
endforeach()
endblock()

add_custom_target(assets DEPENDS ${ASSETS_OUTPUT} SOURCES ${ASSETS_INPUT})

# propagate necessary variables
set(ASSETS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
set(ASSETS_OUTPUT ${ASSETS_OUTPUT} PARENT_SCOPE)

cmake_path(GET CMAKE_CURRENT_BINARY_DIR FILENAME ASSETS_DNAME)

install(
    FILES ${ASSETS_OUTPUT}
    DESTINATION ${HERA_RESOURCE_DIR}/${ASSETS_DNAME}
    COMPONENT Assets
)

# platform-specific icon stuff
add_subdirectory(icons)
