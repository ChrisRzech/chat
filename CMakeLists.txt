cmake_minimum_required(VERSION 3.24)

project(Chat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wconversion -Werror -g)

add_subdirectory(chat) #chat is listed first since apps depends on it
add_subdirectory(apps)
