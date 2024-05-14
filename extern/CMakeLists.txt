### Libraries without CMakeLists

# ==[glad]==
add_library(glad STATIC glad/src/gl.c)
target_include_directories(glad PUBLIC glad/include)

# ==[whereami]==
add_library(whereami STATIC whereami-master/src/whereami.c)
target_include_directories(whereami PUBLIC whereami-master/src)

# ==[stb]==
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb-master)

### Libraries with CMakeLists

set(SKIP_INSTALL_ALL CACHE BOOL "" FORCE)
set(SKIP_INSTALL_LIBRARIES CACHE BOOL "" FORCE)
set(SKIP_INSTALL_HEADERS CACHE BOOL "" FORCE)
add_subdirectory(freetype-VER-2-13-2)
target_compile_definitions(freetype PUBLIC FT_CONFIG_OPTION_ERROR_STRINGS)

# ==[tbb]==
set(TBB_TEST OFF CACHE BOOL "" FORCE)
set(TBB_INSTALL OFF CACHE BOOL "" FORCE)
# tbb must be a shared lib but don't propagate that.
block()
    set(BUILD_SHARED_LIBS ON)
    add_subdirectory(oneTBB-2021.12.0-rc2)
endblock()

# ==[glfw]==
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw-3.4)

# ==[glm]==
add_subdirectory(glm-1.0.1)

# ==[doctest]==
#set(DOCTEST_WITH_MAIN_IN_STATIC_LIB OFF CACHE BOOL "" FORCE)
#set(DOCTEST_NO_INSTALL ON CACHE BOOL "" FORCE)
#add_subdirectory(doctest-2.4.11)

# ==[toml++]==
add_subdirectory(tomlplusplus-3.4.0)

# ==[cpptrace]==
block()
    set(CMAKE_WARN_DEPRECATED FALSE CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS ON)
    add_subdirectory(cpptrace-0.5.2 EXCLUDE_FROM_ALL)
endblock()

# fix for XCode trying to compile .S with `-x c` flag.
if(XCODE)
    get_target_property(libzsrc libzstd_static SOURCES)
    list(FILTER libzsrc INCLUDE REGEX .*\.S)
    cmake_path(SET asmfiles NORMALIZE "${libzsrc}")
    set_source_files_properties(${asmfiles}
        TARGET_DIRECTORY libzstd_static
        PROPERTIES LANGUAGE ASM)
endif()

# ==[boost]==
#add_subdirectory(boost-1.84.0)

# ==[fmt]==
# (must be build before quill
set(FMT_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(fmt-master)

# ==[quill]==
set(QUILL_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
add_subdirectory(quill-3.8.0)
#target_compile_options(quill PRIVATE -Wno-deprecated)

# ==[sol]==
set(SOL2_ENABLE_INSTALL OFF)
set(SOL2_BUILD_LUA FALSE CACHE BOOL "" FORCE)
add_subdirectory(sol2-develop)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/sol2-develop/cmake/Packages"
    "${CMAKE_CURRENT_SOURCE_DIR}/sol2-develop/cmake/Modules")
find_package(LuaBuild REQUIRED COMPONENTS ${SOL2_LUA_VERSION})
