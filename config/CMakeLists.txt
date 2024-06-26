set(CONFIG_INPUT hera.toml.in)

set(CONFIG_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(CONFIG_OUTPUT ${CONFIG_OUTPUT_DIR}/hera.toml)

set(KEYMAP_PATH ${CONFIG_OUTPUT_DIR}/keymap.toml)

cmake_path(RELATIVE_PATH SHADERS_OUTPUT_DIR
    BASE_DIRECTORY ${CONFIG_OUTPUT_DIR}
    OUTPUT_VARIABLE HERA_SHADERS_PATH)

cmake_path(RELATIVE_PATH ASSETS_OUTPUT_DIR
    BASE_DIRECTORY ${CONFIG_OUTPUT_DIR}
    OUTPUT_VARIABLE HERA_ASSETS_PATH)

cmake_path(RELATIVE_PATH KEYMAP_PATH
    BASE_DIRECTORY ${CONFIG_OUTPUT_DIR}
    OUTPUT_VARIABLE HERA_KEYMAP_PATH)

configure_file(${CONFIG_INPUT} ${CONFIG_OUTPUT})

#add_custom_target(config DEPENDS ${CONFIG_OUTPUT} SOURCES ${CONFIG_INPUT})

# propagate necessary variables
set(CONFIG_OUTPUT_DIR ${CONFIG_OUTPUT_DIR} PARENT_SCOPE)
set(CONFIG_OUTPUT ${CONFIG_OUTPUT} PARENT_SCOPE)

cmake_path(GET CMAKE_CURRENT_BINARY_DIR FILENAME CONFIG_DNAME)

install(
    FILES ${CONFIG_OUTPUT}
    DESTINATION ${HERA_RESOURCE_DIR}/${CONFIG_DNAME}
    COMPONENT Config
)


#add_custom_command(
#OUTPUT ${PROJECT_CONFIG_OUTPUT}
#COMMAND "${CMAKE_COMMAND}"
#-D "HERA_SHADERS_PATH=${HERA_SHADERS_PATH}"
#-D "HERA_RESOURCES_PATH=${HERA_RESOURCES_PATH}"
#-P
#"${PROJECT_CONFIG_SCRIPT}"
#"${PROJECT_CONFIG_INPUT}"
#"${PROJECT_CONFIG_OUTPUT}"
#COMMAND "${CMAKE_COMMAND}"
#-E copy -t
#"${PROJECT_EXE_DIR}"
#"${PROJECT_CONFIG_OUTPUT}"
#DEPENDS ${PROJECT_CONFIG_INPUT}
#COMMENT "Configuring ${PROJECT_CONFIG_OUTPUT}"
#VERBATIM)

