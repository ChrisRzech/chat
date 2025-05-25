set(LIBRARY_NAME server)
set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

chat_add_library(${LIBRARY_NAME})

target_sources(${LIBRARY_NAME}
    PRIVATE
        ${SOURCE_PATH}/Connection.cpp
        ${SOURCE_PATH}/ConnectionManager.cpp
        ${SOURCE_PATH}/Listener.cpp
        ${SOURCE_PATH}/RequestHandler.cpp
        ${SOURCE_PATH}/Server.cpp
        ${SOURCE_PATH}/ServerImpl.cpp
)

target_include_directories(${LIBRARY_NAME}
    PUBLIC
        ${HEADER_PATH}
    PRIVATE
        ${SOURCE_PATH}
)

target_link_libraries(${LIBRARY_NAME}
    PRIVATE
        Asio::Asio
        chat::common
        chat::messages
        Threads::Threads
)
