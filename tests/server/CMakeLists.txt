set(TEST_NAME server-test)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

chat_add_test(${TEST_NAME})

target_sources(${TEST_NAME}
    PRIVATE
        ${SOURCE_PATH}/RequestHandlerTest.cpp
)

target_link_libraries(${TEST_NAME} PRIVATE chat::server)

# The test needs to see headers that are defined in the `src` directory. This
# directory is defined in `target_include_directories()`, which adds it to
# `INCLUDE_DIRECTORIES` for the target.
target_include_directories(${TEST_NAME} PRIVATE $<TARGET_PROPERTY:chat::server,INCLUDE_DIRECTORIES>)
