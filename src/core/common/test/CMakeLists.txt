set(TEST_NAME common-test)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

chat_add_test(${TEST_NAME})

target_sources(${TEST_NAME}
    PRIVATE
        ${SOURCE_PATH}/ByteSpanTest.cpp
        ${SOURCE_PATH}/ResultTest.cpp
        ${SOURCE_PATH}/SynchronizedObjectTest.cpp
        ${SOURCE_PATH}/ThreadPoolTest.cpp
        ${SOURCE_PATH}/UtilityTest.cpp
)

target_link_libraries(${TEST_NAME} PRIVATE chat::common)
