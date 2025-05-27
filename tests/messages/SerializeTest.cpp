#include "chat/messages/serialize.hpp"

#include "chat/messages/request/Ping.hpp"

#include "chat/messages/response/Pong.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Using the serializer on a ping request", "[serialize]")
{
    const chat::messages::Ping request;
    auto serialized = chat::messages::serialize(request);
    const chat::common::BufferView bytes{serialized.data(), serialized.size()};
    auto deserialized = chat::messages::deserializeRequest(bytes);
    REQUIRE(deserialized.has_value());
    REQUIRE(deserialized.value() != nullptr);
    REQUIRE(deserialized.value()->getType() ==
            chat::messages::Request::Type::Ping);
}

TEST_CASE("Using the serializer on a pong response", "[serialize]")
{
    const chat::messages::Pong response;
    auto serialized = chat::messages::serialize(response);
    const chat::common::BufferView bytes{serialized.data(), serialized.size()};
    auto deserialized = chat::messages::deserializeResponse(bytes);
    REQUIRE(deserialized.has_value());
    REQUIRE(deserialized.value() != nullptr);
    REQUIRE(deserialized.value()->getType() ==
            chat::messages::Response::Type::Pong);
}
