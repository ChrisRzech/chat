project(messages-test)

set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND SOURCES
    ${SOURCE_PATH}/MessageTest.cpp
    ${SOURCE_PATH}/SerializerTest.cpp
)

find_package(Catch2 3 REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME}
    PRIVATE chat::messages
    PRIVATE Catch2::Catch2WithMain
)

#TODO See apps/Server/CmakeLists.txt for similar issue
set(CMAKE_EXE_LINKER_FLAGS "-static")
