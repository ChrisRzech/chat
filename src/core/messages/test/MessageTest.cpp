#include "chat/messages/Close.hpp"

#include "chat/messages/request/Ping.hpp"

#include "chat/messages/response/Pong.hpp"

#include <catch2/catch_test_macros.hpp>

#include <SFML/Network/Packet.hpp>

SCENARIO("Serializing and deserializing messages", "[Message]")
{
    GIVEN("A close message")
    {
        chat::messages::Close message;

        THEN("The message type is `Close`")
        {
            CHECK(message.getType() == chat::messages::Message::Type::Close);
        }

        WHEN("The message is serialized")
        {
            sf::Packet packet;
            message.serialize(packet);

            AND_WHEN("The message is deserialized")
            {
                decltype(message) deserialized;
                CHECK(deserialized.deserialize(packet));

                THEN(
                    "The deserialized message is the same as the original "
                    "message")
                {
                    // Message does not contain data to check
                }
            }
        }
    }

    GIVEN("A ping request message")
    {
        chat::messages::Ping message;

        THEN("The message type is `Request`")
        {
            CHECK(message.Message::getType() ==
                  chat::messages::Message::Type::Request);
        }

        AND_THEN("The request type is `Ping`")
        {
            CHECK(message.getType() == chat::messages::Request::Type::Ping);
        }

        WHEN("The message is serialized")
        {
            sf::Packet packet;
            message.serialize(packet);

            AND_WHEN("The message is deserialized")
            {
                decltype(message) deserialized;
                CHECK(deserialized.deserialize(packet));

                THEN(
                    "The deserialized message is the same as the original "
                    "message")
                {
                    // Message does not contain data to check
                }
            }
        }
    }

    GIVEN("A pong response message")
    {
        chat::messages::Pong message;

        THEN("The message type is `Response`")
        {
            CHECK(message.Message::getType() ==
                  chat::messages::Message::Type::Response);
        }

        AND_THEN("The request type is `Pong`")
        {
            CHECK(message.getType() == chat::messages::Response::Type::Pong);
        }

        WHEN("The message is serialized")
        {
            sf::Packet packet;
            message.serialize(packet);

            AND_WHEN("The message is deserialized")
            {
                decltype(message) deserialized;
                CHECK(deserialized.deserialize(packet));

                THEN(
                    "The deserialized message is the same as the original "
                    "message")
                {
                    // Message does not contain data to check
                }
            }
        }
    }
}
