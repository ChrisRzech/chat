#include "chat/messages/Close.hpp"
#include "chat/messages/serialize.hpp"

#include "chat/messages/request/Ping.hpp"

#include "chat/messages/response/Pong.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Using the serializer on a close message", "[serialize]")
{
    const chat::messages::Close message;
    auto serialized = chat::messages::serialize(message);
    const chat::common::ByteSpan bytes{serialized.data(), serialized.size()};
    auto deserialized = chat::messages::deserialize(bytes);
    REQUIRE(deserialized.has_value());
    REQUIRE(deserialized.value() != nullptr);
    REQUIRE(deserialized.value()->getMessageType() ==
            chat::messages::Message::Type::Close);
}

TEST_CASE("Using the serializer on a ping request message", "[serialize]")
{
    const chat::messages::Ping message;
    auto serialized = chat::messages::serialize(message);
    const chat::common::ByteSpan bytes{serialized.data(), serialized.size()};
    auto deserialized = chat::messages::deserialize(bytes);
    REQUIRE(deserialized.has_value());
    REQUIRE(deserialized.value() != nullptr);
    REQUIRE(deserialized.value()->getMessageType() ==
            chat::messages::Message::Type::Request);

    auto casted = dynamic_cast<decltype(message)*>(deserialized.value().get());
    REQUIRE(casted != nullptr);
    REQUIRE(casted->getRequestType() == message.getRequestType());
}

TEST_CASE("Using the serializer on a pong response message", "[serialize]")
{
    const chat::messages::Pong message;
    auto serialized = chat::messages::serialize(message);
    const chat::common::ByteSpan bytes{serialized.data(), serialized.size()};
    auto deserialized = chat::messages::deserialize(bytes);
    REQUIRE(deserialized.has_value());
    REQUIRE(deserialized.value() != nullptr);
    REQUIRE(deserialized.value()->getMessageType() ==
            chat::messages::Message::Type::Response);

    auto casted = dynamic_cast<decltype(message)*>(deserialized.value().get());
    REQUIRE(casted != nullptr);
    REQUIRE(casted->getResponseType() == message.getResponseType());
}
