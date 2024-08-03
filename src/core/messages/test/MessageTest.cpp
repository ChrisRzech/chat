#include "chat/common/InputByteStream.hpp"
#include "chat/common/OutputByteStream.hpp"

#include "chat/messages/Close.hpp"

#include "chat/messages/request/Ping.hpp"

#include "chat/messages/response/Pong.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Creating a close message", "[Message]")
{
    const chat::messages::Close message;
    REQUIRE(message.getMessageType() == chat::messages::Message::Type::Close);
}

TEST_CASE("Serializing and deserializing a close message", "[Message]")
{
    const chat::messages::Close message;

    chat::common::OutputByteStream out;
    message.serialize(out);
    const auto& serialized = out.getData();

    chat::common::InputByteStream in{
        chat::common::ByteSpan{serialized.data(), serialized.size()}};
    chat::messages::Close deserialized;
    REQUIRE(deserialized.deserialize(in));
}

TEST_CASE("Creating a ping request message", "[Message]")
{
    const chat::messages::Ping message;
    REQUIRE(message.getMessageType() == chat::messages::Message::Type::Request);
    REQUIRE(message.getRequestType() == chat::messages::Request::Type::Ping);
}

TEST_CASE("Serializing and deserializing a ping request message", "[Message]")
{
    const chat::messages::Ping message;

    chat::common::OutputByteStream out;
    message.serialize(out);
    const auto& serialized = out.getData();

    chat::common::InputByteStream in{
        chat::common::ByteSpan{serialized.data(), serialized.size()}};
    chat::messages::Ping deserialized;
    REQUIRE(deserialized.deserialize(in));
}

TEST_CASE("Creating a pong response message", "[Message]")
{
    const chat::messages::Pong message;
    REQUIRE(message.getMessageType() ==
            chat::messages::Message::Type::Response);
    REQUIRE(message.getResponseType() == chat::messages::Response::Type::Pong);
}

TEST_CASE("Serializing and deserializing a pong response message", "[Message]")
{
    const chat::messages::Pong message;

    chat::common::OutputByteStream out;
    message.serialize(out);
    const auto& serialized = out.getData();

    chat::common::InputByteStream in{
        chat::common::ByteSpan{serialized.data(), serialized.size()}};
    chat::messages::Pong deserialized;
    REQUIRE(deserialized.deserialize(in));
}
