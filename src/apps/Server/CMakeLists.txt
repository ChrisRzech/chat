set(APP_NAME server-app)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

chat_add_app(${APP_NAME})

target_sources(${APP_NAME}
    PRIVATE ${SOURCE_PATH}/main.cpp
)

target_link_libraries(${APP_NAME}
    PRIVATE chat::common
    PRIVATE chat::server
)
