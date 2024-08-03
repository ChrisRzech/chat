#include "RequestHandler.hpp"

#include "chat/messages/request/Ping.hpp"

#include "chat/messages/response/Pong.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Handling a ping request", "[RequestHandler]")
{
    chat::server::RequestHandler handler;
    const chat::messages::Ping request;
    auto response = handler.handle(request);
    REQUIRE(response != nullptr);
    auto* casted = dynamic_cast<const chat::messages::Pong*>(response.get());
    REQUIRE(casted != nullptr);
}
