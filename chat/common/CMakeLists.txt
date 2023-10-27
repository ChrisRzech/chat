project(common)

set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

list(APPEND SOURCES
    ${SOURCE_PATH}/Logging.cpp
    ${SOURCE_PATH}/ThreadPool.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_library(chat::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_PATH})
