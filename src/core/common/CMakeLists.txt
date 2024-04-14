set(LIBRARY_NAME common)
set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

chat_add_library(${LIBRARY_NAME})

target_sources(${LIBRARY_NAME}
    PRIVATE
        ${SOURCE_PATH}/ByteSpan.cpp
        ${SOURCE_PATH}/Logging.cpp
        ${SOURCE_PATH}/ThreadPool.cpp
)

target_include_directories(${LIBRARY_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${HEADER_PATH}>
)

add_subdirectory(test)
