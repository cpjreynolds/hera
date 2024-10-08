file(GLOB_RECURSE HERA_HEADERS CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE HERA_SOURCES CONFIGURE_DEPENDS *.cpp)
set(HERA_TESTS ${HERA_SOURCES})
#list(FILTER PROJECT_TESTS EXCLUDE REGEX .*\/main.cpp)

# abstract dependencies
set(HERA_DEPENDENCIES
    shaders
    assets)

# concrete binary dependencies
set(HERA_LIBRARIES
    glfw
    glm
    glad
    tomlplusplus::tomlplusplus
    whereami
    stb
    TBB::tbb
    TBB::tbbmalloc
    TBB::tbbmalloc_proxy
    quill::quill
    Lua::Lua
    sol2::sol2
    freetype
)

list(APPEND HERA_DEFINES
    GLFW_INCLUDE_NONE
    QUILL_ROOT_LOGGER_ONLY
    SOL_ALL_SAFETIES_ON
    SOL_PRINT_ERRORS=0
    SOL_USING_CXX_LUA=0
    SOL_USE_LUA_HPP=0
    SOL_USING_CXX_LUAJIT=0
)

if (${CMAKE_CXX_BYTE_ORDER} STREQUAL "BIG_ENDIAN")
    list(APPEND HERA_DEFINES HERA_BIG_ENDIAN)
elseif(${CMAKE_CXX_BYTE_ORDER} STREQUAL "LITTLE_ENDIAN")
    list(APPEND HERA_DEFINES HERA_LITTLE_ENDIAN)
endif()

if (HERA_STACKTRACE)
    list(APPEND HERA_LIBRARIES cpptrace::cpptrace)
endif()

foreach(opt ${HERA_OPTIONS})
    list(APPEND HERA_DEFINES ${opt}=$<BOOL:${${opt}}>)
endforeach()

# easier than relying on compiler-specific defines
if(WIN32)
    list(APPEND HERA_DEFINES WINDOWS)
endif()

if(UNIX)
    list(APPEND HERA_DEFINES UNIX) # includes macos and cygwin
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    list(APPEND HERA_DEFINES DARWIN)
endif()

if(NOT MSVC AND NOT WIN32)
    list(APPEND HERA_LIBRARIES dl)
endif()

# propagate stuff upwards
list(APPEND PROJECT_LIBRARIES ${HERA_LIBRARIES})
set(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} PARENT_SCOPE)

add_executable(hera ${HERA_SOURCES} ${HERA_HEADERS})

# the main executable never has doctests
target_compile_definitions(hera PUBLIC DOCTEST_CONFIG_DISABLE)

# interface for compile options shared between targets
# add_library(hera_common INTERFACE)

# target_compile_features(hera_common INTERFACE cxx_std_23 c_std_11)
# target_include_directories(hera_common INTERFACE ${PROJECT_SOURCE_DIR})
# target_compile_definitions(hera_common INTERFACE ${HERA_DEFINES})
# target_link_libraries(hera_common INTERFACE ${HERA_LIBRARIES})
# add_dependencies(hera_common INTERFACE ${HERA_DEPENDENCIES})

target_compile_features(hera PUBLIC cxx_std_23 c_std_11)

target_include_directories(hera PUBLIC ${PROJECT_SOURCE_DIR})

target_compile_definitions(hera PRIVATE ${HERA_DEFINES})

target_link_libraries(hera PUBLIC ${HERA_LIBRARIES})
add_dependencies(hera ${HERA_DEPENDENCIES})

if(MSVC)
    target_compile_options(hera PUBLIC /W4)
else()
    target_compile_options(hera PUBLIC -Wall -Wextra -fno-char8_t)
endif()

#target_link_libraries(hera PUBLIC hera_common)

#if(BUILD_TESTING)
#include(${PROJECT_SOURCE_DIR}/extern/doctest-2.4.11/scripts/cmake/doctest.cmake)
#add_executable(testbin ${HERA_TESTS} ${HERA_HEADERS})
#target_link_libraries(testbin hera_common doctest)
#doctest_discover_tests(testbin)
#endif()

add_custom_target(
    run
    COMMAND "$<TARGET_FILE:${PROJECT_NAME}>"
    DEPENDS hera
    VERBATIM)

if (HERA_BUNDLE)
add_custom_target(
    runbundle
    COMMAND open $<TARGET_BUNDLE_DIR:${PROJECT_NAME}> -o [=[`tty`]=]
    DEPENDS hera
    )
endif()

if(WIN32)
    set(tracetgt "$<TARGET_FILE:cpptrace::cpptrace>")
    set(tracedest "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    set(tcmd ${CMAKE_COMMAND} -E copy -t ${tracedest} ${tracetgt})
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${tcmd}
    COMMENT "Copying cpptrace.dll"
    VERBATIM COMMAND_EXPAND_LISTS
    )
endif()

# for ycm semantic completion support (debug is the most inclusive)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${PROJECT_BINARY_DIR}/compile_commands.json
            ${PROJECT_SOURCE_DIR}/compile_commands.json
        DEPENDS ${PROJECT_BINARY_DIR}/compile_commands.json)
endif()


