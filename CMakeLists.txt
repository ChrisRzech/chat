cmake_minimum_required(VERSION 3.18)

include(cmake/Macros.cmake)

#Set CMAKE_BUILD_TYPE
if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
    message(STATUS "Defaulting CMAKE_BUILD_TYPE to '${CMAKE_BUILD_TYPE}'")
elseif(NOT ${CMAKE_BUILD_TYPE} MATCHES "Debug|RelWithDebInfo|Release")
    message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'")
endif()
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Project build type (Debug, RelWithDebInfo, or Release)" FORCE)
message(STATUS "CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'")

project(chat LANGUAGES CXX)

add_subdirectory(src)
