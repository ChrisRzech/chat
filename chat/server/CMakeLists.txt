project(server)

set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

list(APPEND SOURCES
    ${SOURCE_PATH}/Connection.cpp
    ${SOURCE_PATH}/Server.cpp
)

find_package(SFML 2.5 REQUIRED COMPONENTS network)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_library(chat::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_PATH})
target_link_libraries(${PROJECT_NAME}
    PUBLIC sfml-network
    PRIVATE chat::common
    PRIVATE chat::messages
)
