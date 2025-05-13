set(LIBRARY_NAME common)
set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

chat_add_library(${LIBRARY_NAME})

target_sources(${LIBRARY_NAME}
    PRIVATE
        ${SOURCE_PATH}/ByteSpan.cpp
        ${SOURCE_PATH}/InputByteStream.cpp
        ${SOURCE_PATH}/Logging.cpp
        ${SOURCE_PATH}/OutputByteStream.cpp
        ${SOURCE_PATH}/ThreadPool.cpp
        ${SOURCE_PATH}/utility.cpp
)

target_include_directories(${LIBRARY_NAME}
    PUBLIC
        ${HEADER_PATH}
)

set(THREADS_PREFER_PTHREAD_FLAG true)
find_package(Threads REQUIRED)

target_link_libraries(${LIBRARY_NAME}
    PRIVATE
        Threads::Threads
)
