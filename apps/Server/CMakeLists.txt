project(Server)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE chat::chatserver)

#TODO For some reason, running the executable in Git Bash doesn't seem to work correctly. The supposedly "static" chatserver library doesn't
#seem to be statically linked into the executable. This might be a Windows thing (and it probably is), so the "-static" linking flag
#"forces" the chat::chatserver library to be linked. For now, this flag will be used just for making development until there is a proper
#solution to handle this.
set(CMAKE_EXE_LINKER_FLAGS "-static")
