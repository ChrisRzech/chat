cmake_minimum_required(VERSION 3.24)

project(Chat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wconversion -Werror -g)

#TODO Check if the ordering matters... After testing it, ordering doesn't seem to matter, but keeping it just in case.
#The ordering of adding subdirectories depends on who is dependent on who
add_subdirectory(chat) #chat is listed first since apps depends on it
add_subdirectory(apps)
