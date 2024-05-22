#include "RequestHandler.hpp"

#include "chat/messages/request/Ping.hpp"

#include "chat/messages/response/Pong.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("Handling requests", "[RequestHandler]")
{
    GIVEN("A request handler")
    {
        chat::server::RequestHandler handler;

        WHEN("Handling a ping request")
        {
            const chat::messages::Ping request;
            auto response = handler.handle(request);

            THEN("A pong response is provided")
            {
                REQUIRE(response != nullptr);

                auto casted =
                    dynamic_cast<chat::messages::Pong*>(response.get());
                REQUIRE(casted != nullptr);
            }
        }
    }
}
