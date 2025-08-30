cmake_minimum_required (VERSION 3.28)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(
    hera
    VERSION 0.0.1
    DESCRIPTION "a project named hera"
    LANGUAGES C CXX
)

# project-wide includes
include(GNUInstallDirs)
# feature printing
include(FeatureSummary)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ProjectUtility)

set_default_build()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# build process options
option(HERA_INSTALL "enable install directives" ON)
option(HERA_BUNDLE "build as a .app" ${APPLE})
option(HERA_TEST "build tests" OFF)
option(HERA_CHECK_COPYRIGHT "check source file copyright notices" ON)
option(HERA_DELETE_DS_STORE "delete .DS_Store files" ${APPLE})

# build options
hera_option(CATCH "top-level try/catch block" OFF)
hera_option(STACKTRACE "stacktracing exceptions" ON)
hera_option(NICEABORT "terminate with exit(0)" ON)
hera_option(APPLE_FUCKERY "fix apple VSYNC cockups" ${APPLE})
hera_option(DEBUG "debug features" ON)

set(HERA_SCRIPT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/scripts")

if(HERA_BUNDLE)
    set(HERA_RESOURCE_DIR "$<TARGET_BUNDLE_DIR_NAME:hera>/Contents/Resources")
else()
    set(HERA_RESOURCE_DIR "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")
endif()

if(HERA_CHECK_COPYRIGHT)
    find_package(Python COMPONENTS Interpreter REQUIRED)
    add_custom_target(check_copyright ALL
        COMMAND ${Python_EXECUTABLE} ${HERA_SCRIPT_DIR}/add_copyright.py --check
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "check copyright"
    )

    add_custom_target(add_copyright
        COMMAND ${Python_EXECUTABLE} ${HERA_SCRIPT_DIR}/add_copyright.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "add copyright"
    )
endif()

set(BUILD_TESTING ${HERA_TEST})

# nothing to test yet
# if (PROJECT_IS_TOP_LEVEL)
    # # disables generation of all the CTest targets
    # set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
    # include(CTest)
# endif()

# youcompleteme/clangd support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# global compiler options (for codegen flags)

if(MSVC)
    # placeholder
else()
    add_compile_options(-march=native)
endif()

# external dependencies
add_subdirectory(extern)

# ==[project]==
add_subdirectory(shaders)
add_subdirectory(assets)
add_subdirectory(config)

add_subdirectory(hera)

feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES)

# get the list of shared libraries to package
block(PROPAGATE PROJECT_SHARELIBS PROJECT_SHAREDIRS)
foreach(lib ${PROJECT_LIBRARIES})
    if(TARGET ${lib})
        get_target_property(_type ${lib} TYPE)
        if(_type STREQUAL "SHARED_LIBRARY")
            get_target_property(aliased ${lib} ALIASED_TARGET)
            get_target_property(libdir ${lib} LIBRARY_OUTPUT_DIRECTORY)
            get_target_property(bindir ${lib} BINARY_DIR)
            list(APPEND PROJECT_SHAREDIRS ${libdir} ${bindir})
            if(aliased)
                list(APPEND PROJECT_SHARELIBS ${aliased})
            else()
                list(APPEND PROJECT_SHARELIBS ${lib})
            endif()
        endif()
    endif()
    list(REMOVE_DUPLICATES PROJECT_SHAREDIRS)
    list(FILTER PROJECT_SHAREDIRS EXCLUDE REGEX ".*NOTFOUND")
endforeach()
endblock()


# installation and packaging

set(CPACK_GENERATOR ZIP TGZ)

if(HERA_BUNDLE)
    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${HERA_SCRIPT_DIR}/MacOSXBundleInfo.plist.in"
        MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_COPYRIGHT "© 2024 Cole Reynolds"
        MACOSX_BUNDLE_INFO_STRING "hera"
        #MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_ICON_FILE "icon.icns"
    )

    install(TARGETS hera
        BUNDLE DESTINATION . COMPONENT Runtime)

    install(CODE
        "include(BundleUtilities)
        fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/hera.app\" \"\"
            \"${PROJECT_SHAREDIRS}\")")

    list(APPEND CPACK_GENERATOR DragNDrop)
else()
    install(TARGETS hera COMPONENT Runtime)
    install(IMPORTED_RUNTIME_ARTIFACTS ${PROJECT_SHARELIBS} COMPONENT Sharelibs)
endif()

get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
list(FILTER CPACK_COMPONENTS_ALL EXCLUDE REGEX "^.*cpptrace.*$")

include(CPack)
