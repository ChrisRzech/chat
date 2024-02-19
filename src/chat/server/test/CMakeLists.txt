set(TEST_NAME server-test)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

chat_add_test(${TEST_NAME})

target_sources(${TEST_NAME}
    PRIVATE
        ${SOURCE_PATH}/RequestHandlerTest.cpp
)

target_link_libraries(${TEST_NAME} PRIVATE chat::server)

#Add private includes and dependency libraries of the library being tested
target_include_directories(${TEST_NAME} PRIVATE $<TARGET_PROPERTY:chat::server,INCLUDE_DIRECTORIES>)
target_link_libraries(${TEST_NAME} PRIVATE $<TARGET_PROPERTY:chat::server,LINK_LIBRARIES>)
