project(Client)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(${PROJECT_NAME}
    PRIVATE chat::client
    PRIVATE chat::common
)

#TODO See apps/Server/CmakeLists.txt for similar issue
set(CMAKE_EXE_LINKER_FLAGS "-static")
