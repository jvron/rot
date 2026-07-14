cmake_minimum_required(VERSION 3.25)

project(
    Rot
    VERSION 0.1.0
    DESCRIPTION "Rot Programming Language"
    LANGUAGES CXX
)


#
# C++ Standard
#

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


#
# Build options
#

option(ROT_BUILD_COMPILER "Build Rot compiler" ON)
option(ROT_BUILD_VM "Build Rot VM" ON)
option(ROT_BUILD_RUNTIME "Build Rot runtime" ON)

option(ROT_BUILD_TESTS "Build tests" OFF)
option(ROT_ENABLE_WARNINGS "Enable compiler warnings" ON)

option(ROT_ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(ROT_ENABLE_LTO "Enable Link Time Optimization" OFF)


#
# Default build type
#

if(NOT CMAKE_BUILD_TYPE)
    set(
        CMAKE_BUILD_TYPE Debug
        CACHE STRING "Build type"
        FORCE
    )
endif()


#
# CMake modules
#

list(APPEND CMAKE_MODULE_PATH
    "${PROJECT_SOURCE_DIR}/cmake"
)

include(BuildOptions)


#
# Targets
#

if(ROT_BUILD_RUNTIME)

    add_subdirectory(runtime/rotrt)

endif()


if(ROT_BUILD_VM)

    add_subdirectory(vm/rotvm)

endif()


if(ROT_BUILD_COMPILER)

    add_subdirectory(compiler/rotc)

endif()


#
# Tests
#

if(ROT_BUILD_TESTS)

    enable_testing()

    add_subdirectory(tests)

endif()