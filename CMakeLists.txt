cmake_minimum_required(VERSION 3.18)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Project build type" FORCE)
endif()
message(STATUS "CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'")

project(chat LANGUAGES CXX)
include(macros)
include(options)
# Include CTest at the top level directory to enable running the `ctest` command
# at the top level of the build directory
include(CTest)

add_subdirectory(src)
add_subdirectory(tests)
