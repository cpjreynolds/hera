function(make_copy_commands)
    set(singlevalue INPUT OUTPUT)
    cmake_parse_arguments(arg "" "${singlevalue}" "" ${ARGN})
    foreach(item ${${arg_INPUT}})
        set(infile "${CMAKE_CURRENT_SOURCE_DIR}/${item}")
        set(outfile "${CMAKE_CURRENT_BINARY_DIR}/${item}")
        add_custom_command(
            OUTPUT ${outfile}
            COMMAND ${CMAKE_COMMAND} -E copy ${infile} ${outfile}
            MAIN_DEPENDENCY ${infile}
            COMMENT "copying: ${item}" VERBATIM COMMAND_EXPAND_LISTS)
        list(APPEND ${arg_OUTPUT} "${outfile}")
    endforeach()
    return(PROPAGATE ${arg_OUTPUT})
endfunction()

list(APPEND ASSETS_EXCLUDES ".*CMakeLists\.txt$")
list(APPEND ASSETS_EXCLUDES ".*\.DS_Store$")

function(glob_assets var)
    set(multivalue EXCLUDE INCLUDE)
    cmake_parse_arguments(arg "" "" "${multivalue}" ${ARGN})
    list(APPEND ASSETS_EXCLUDES ${arg_EXCLUDE})
    file(GLOB_RECURSE ${var} RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        CONFIGURE_DEPENDS ${arg_INCLUDE})
    foreach(item ${ASSETS_EXCLUDES})
        list(FILTER ${var} EXCLUDE REGEX ${item})
    endforeach()
    return(PROPAGATE ${var})
endfunction()

function(add_asset name)
    set(multivalue DEPENDS SOURCES)
    cmake_parse_arguments(arg "" "" "${multivalue}" ${ARGN})
    string(TOLOWER ${name} tgt_lower)
    string(TOUPPER ${name} tgt_upper)
    set(outdir ${tgt_upper}_OUTPUT_DIR)
    set(installdir ${tgt_upper}_INSTALL_DIR)
    set(${outdir} ${CMAKE_CURRENT_BINARY_DIR})
    cmake_path(RELATIVE_PATH CMAKE_CURRENT_BINARY_DIR BASE_DIRECTORY
        ASSETS_OUTPUT_DIR OUTPUT_VARIABLE dname)
    set(${installdir} ${ASSETS_INSTALL_DIR}/${dname})

    add_custom_target(${tgt_lower} DEPENDS ${arg_DEPENDS} SOURCES
        ${arg_SOURCES})

    string(SUBSTRING "${tgt_lower}" 0 1 _first)
    string(SUBSTRING "${tgt_lower}" 1 -1 _rest)
    string(TOUPPER "${_first}" _first_up)
    set(compname "${_first_up}${_rest}")

    install(
        FILES ${arg_DEPENDS}
        DESTINATION ${installdir}
        COMPONENT ${compname}
    )
    return(PROPAGATE ${outdir} ${installdir})
endfunction()



# propagate necessary variables
set(ASSETS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(ASSETS_INSTALL_DIR ${HERA_RESOURCES_DIR}/assets)

# platform-specific icon stuff
add_subdirectory(icons)
add_subdirectory(data)
add_subdirectory(shaders)
add_subdirectory(fonts)

add_custom_target(assets)
add_dependencies(assets data fonts shaders)

return(PROPAGATE ASSETS_OUTPUT_DIR ASSETS_INSTALL_DIR)
