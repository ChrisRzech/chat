set(LIBRARY_NAME client)
set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(SFML 2.5 REQUIRED COMPONENTS network)

chat_add_library(${LIBRARY_NAME})

target_sources(${LIBRARY_NAME}
    PRIVATE
        ${SOURCE_PATH}/Client.cpp
)

target_include_directories(${LIBRARY_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${HEADER_PATH}>
)

target_link_libraries(${LIBRARY_NAME}
    PRIVATE
        chat::common
        chat::messages
        sfml-network
)
