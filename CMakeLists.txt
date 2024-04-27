cmake_minimum_required(VERSION 3.18)

include(cmake/Macros.cmake)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Project build type" FORCE)
endif()
message(STATUS "CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'")

option(CHAT_ENABLE_WARNING_AS_ERROR "Convert compiler warnings into errors" off)
message(STATUS "CHAT_ENABLE_WARNING_AS_ERROR: '${CHAT_ENABLE_WARNING_AS_ERROR}'")

project(chat LANGUAGES CXX)

add_subdirectory(src)
