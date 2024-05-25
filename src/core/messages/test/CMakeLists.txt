set(TEST_NAME messages-test)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

chat_add_test(${TEST_NAME})

target_sources(${TEST_NAME}
    PRIVATE
        ${SOURCE_PATH}/MessageTest.cpp
        ${SOURCE_PATH}/SerializeTest.cpp
)

target_link_libraries(${TEST_NAME} PRIVATE chat::messages)
