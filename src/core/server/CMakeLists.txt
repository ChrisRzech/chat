set(LIBRARY_NAME server)
set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

chat_add_library(${LIBRARY_NAME})

target_sources(${LIBRARY_NAME}
    PRIVATE
        ${SOURCE_PATH}/Connection.cpp
        ${SOURCE_PATH}/RequestHandler.cpp
        ${SOURCE_PATH}/Server.cpp
        ${SOURCE_PATH}/ServerImpl.cpp
        ${SOURCE_PATH}/StateManager.cpp
)

target_include_directories(${LIBRARY_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${HEADER_PATH}>
    PRIVATE
        ${SOURCE_PATH}
)

find_package(SFML 2.5 REQUIRED COMPONENTS network)
set(THREADS_PREFER_PTHREAD_FLAG true)
find_package(Threads REQUIRED)

target_link_libraries(${LIBRARY_NAME}
    PRIVATE
        chat::common
        chat::messages
        sfml-network
        Threads::Threads
)

add_subdirectory(test)
