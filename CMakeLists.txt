cmake_minimum_required(VERSION 3.18)

include(cmake/Macros.cmake)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Project build type" FORCE)
endif()
message(STATUS "CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'")

option(CHAT_ENABLE_WARNING_AS_ERROR "Convert compiler warnings into errors" off)
message(STATUS "CHAT_ENABLE_WARNING_AS_ERROR: '${CHAT_ENABLE_WARNING_AS_ERROR}'")

option(CHAT_ENABLE_ASAN "Enable address sanitizer" off)
message(STATUS "CHAT_ENABLE_ASAN: '${CHAT_ENABLE_ASAN}'")

option(CHAT_ENABLE_TSAN "Enable thread sanitizer" off)
message(STATUS "CHAT_ENABLE_TSAN: '${CHAT_ENABLE_TSAN}'")

option(CHAT_ENABLE_UBSAN "Enable undefined sanitizer" off)
message(STATUS "CHAT_ENABLE_UBSAN: '${CHAT_ENABLE_UBSAN}'")

option(CHAT_ENABLE_LSAN "Enable leak sanitizer" off)
message(STATUS "CHAT_ENABLE_LSAN: '${CHAT_ENABLE_LSAN}'")

project(chat LANGUAGES CXX)

add_subdirectory(src)
