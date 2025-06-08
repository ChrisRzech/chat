#include "chat/common/InputByteStream.hpp"
#include "chat/common/OutputByteStream.hpp"
#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"
#include "chat/messages/request/Ping.hpp"
#include "chat/messages/response/Pong.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Creating a ping request", "[Message]")
{
    const chat::messages::Ping request;
    REQUIRE(request.getType() == chat::messages::Request::Type::Ping);
}

TEST_CASE("Serializing and deserializing a ping request", "[Message]")
{
    const chat::messages::Ping request;

    chat::common::OutputByteStream out;
    request.serialize(out);
    const auto& serialized = out.getData();

    chat::common::InputByteStream in{
        chat::common::BufferView{serialized.data(), serialized.size()}};
    chat::messages::Ping deserialized;
    REQUIRE(deserialized.deserialize(in));
}

TEST_CASE("Creating a pong response", "[Message]")
{
    const chat::messages::Pong response;
    REQUIRE(response.getType() == chat::messages::Response::Type::Pong);
}

TEST_CASE("Serializing and deserializing a pong response", "[Message]")
{
    const chat::messages::Pong response;

    chat::common::OutputByteStream out;
    response.serialize(out);
    const auto& serialized = out.getData();

    chat::common::InputByteStream in{
        chat::common::BufferView{serialized.data(), serialized.size()}};
    chat::messages::Pong deserialized;
    REQUIRE(deserialized.deserialize(in));
}
