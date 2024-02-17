project(server-test)

set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
    ${SOURCE_PATH}/RequestHandlerTest.cpp
)

find_package(Catch2 3 REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME}
    PRIVATE chat::server
    PRIVATE Catch2::Catch2WithMain
)

#Add private includes and dependency includes of tested component
get_target_property(component_link_libs chat::server LINK_LIBRARIES)
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_PATH}/../src)
target_link_libraries(${PROJECT_NAME} PRIVATE ${component_link_libs})

#TODO See apps/Server/CmakeLists.txt for similar issue
set(CMAKE_EXE_LINKER_FLAGS "-static")
