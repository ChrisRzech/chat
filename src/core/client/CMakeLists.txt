set(LIBRARY_NAME client)
set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

chat_add_library(${LIBRARY_NAME})

target_sources(${LIBRARY_NAME}
    PRIVATE
        ${SOURCE_PATH}/Client.cpp
)

target_include_directories(${LIBRARY_NAME}
    PUBLIC
        ${HEADER_PATH}
)

target_link_libraries(${LIBRARY_NAME}
    PUBLIC
        chat::common
    PRIVATE
        chat::messages
        sfml-network
        Threads::Threads
)
