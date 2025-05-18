cmake_minimum_required(VERSION 3.18)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(cmake/macros.cmake)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Project build type" FORCE)
endif()
message(STATUS "CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'")

chat_option(CHAT_ENABLE_WARNING_AS_ERROR "Convert compiler warnings into errors" off)
chat_option(CHAT_ENABLE_ASAN "Enable address sanitizer" off)
chat_option(CHAT_ENABLE_TSAN "Enable thread sanitizer" off)
chat_option(CHAT_ENABLE_UBSAN "Enable undefined sanitizer" off)
chat_option(CHAT_ENABLE_LSAN "Enable leak sanitizer" off)

project(chat LANGUAGES CXX)

# Include CTest at the top level directory to enable running the `ctest` command
# at the top level of the build directory
include(CTest)

add_subdirectory(src)
add_subdirectory(tests)
