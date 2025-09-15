find_package(Vulkan COMPONENTS glslc REQUIRED)

# file(GLOB SHADERS_INPUT RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS
    # *.vert
    # *.frag
    # *.geom
    # *.comp)
#
#
#
# file(GLOB SHADERS_INCLUDES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    # CONFIGURE_DEPENDS
    # *.glsl)

glob_assets(SHADERS_INPUT INCLUDE *.vert *.frag *.geom *.comp)
glob_assets(SHADERS_INCLUDES INCLUDE *.glsl)


block(PROPAGATE SHADERS_OUTPUT)
foreach(shader ${SHADERS_INPUT})
    set(infile "${CMAKE_CURRENT_SOURCE_DIR}/${shader}")
    set(outfile "${CMAKE_CURRENT_BINARY_DIR}/${shader}")
    add_custom_command(
        OUTPUT ${outfile}
        # depfile needs absolute paths
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} -M
            -o  ${outfile}.d
                ${infile}
            -MT ${outfile}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} -E
            -o ${outfile}
               ${shader} # relative path for nicer #line annotations
        MAIN_DEPENDENCY
            ${infile}
        DEPFILE
            ${outfile}.d
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR} # necessary for the relative path
        COMMENT "preprocessing: ${shader}" VERBATIM COMMAND_EXPAND_LISTS)
    list(APPEND SHADERS_OUTPUT "${outfile}")
endforeach()
endblock()

add_asset(shaders DEPENDS ${SHADERS_OUTPUT} SOURCES ${SHADERS_INPUT}
    ${SHADERS_INCLUDES})

return(PROPAGATE SHADERS_OUTPUT_DIR)

# add_custom_target(shaders
    # DEPENDS ${SHADERS_OUTPUT}
    # SOURCES ${SHADERS_INPUT} ${SHADERS_INCLUDES})
#
# set(SHADERS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
# set(SHADERS_OUTPUT ${SHADERS_OUTPUT} PARENT_SCOPE)
#
# cmake_path(RELATIVE_PATH CMAKE_CURRENT_BINARY_DIR BASE_DIRECTORY
    # ${ASSETS_OUTPUT_DIR} OUTPUT_VARIABLE SHADERS_INSTALL_DIR)
#
# cmake_path(GET CMAKE_CURRENT_BINARY_DIR FILENAME SHADERS_DNAME)
#
# install(
    # FILES ${SHADERS_OUTPUT}
    # DESTINATION ${SHADERS_INSTALL_DIR}
    # COMPONENT Shaders
# )

