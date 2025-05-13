set(LIBRARY_NAME messages)
set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(SFML 2.5 REQUIRED COMPONENTS network)

chat_add_library(${LIBRARY_NAME})

target_sources(${LIBRARY_NAME}
    PRIVATE
        ${SOURCE_PATH}/IncrementalRequestDeserializer.cpp
        ${SOURCE_PATH}/Request.cpp
        ${SOURCE_PATH}/Response.cpp
        ${SOURCE_PATH}/serialize.cpp
        ${SOURCE_PATH}/request/Ping.cpp
        ${SOURCE_PATH}/response/Pong.cpp
)

target_include_directories(${LIBRARY_NAME}
    PUBLIC
        ${HEADER_PATH}
)

target_link_libraries(${LIBRARY_NAME}
    PUBLIC
        chat::common
)
