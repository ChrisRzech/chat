#include "chat/messages/Close.hpp"
#include "chat/messages/Serializer.hpp"

#include "chat/messages/request/Ping.hpp"

#include "chat/messages/response/Pong.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("Using the serializer to serializing and deserializing messages", "[Serializer]")
{
    GIVEN("A serializer")
    {
        chat::messages::Serializer serializer;

        AND_GIVEN("A close message")
        {
            chat::messages::Close message;

            WHEN("The message is serialized using the serializer")
            {
                auto serialized = serializer.serialize(message);

                AND_WHEN("The message is deserialized using the serializer")
                {
                    auto deserialized = serializer.deserialize(serialized);

                    THEN("The deserialized message is the same as the original message")
                    {
                        REQUIRE(deserialized.has_value());
                        REQUIRE(deserialized.value() != nullptr);
                        CHECK(deserialized.value()->getType() == chat::messages::Message::Type::Close);
                    }
                }
            }
        }

        AND_GIVEN("A ping request message")
        {
            chat::messages::Ping message;

            WHEN("The message is serialized using the serializer")
            {
                auto serialized = serializer.serialize(message);

                AND_WHEN("The message is deserialized using the serializer")
                {
                    auto deserialized = serializer.deserialize(serialized);

                    THEN("The deserialized message is the same as the original message")
                    {
                        REQUIRE(deserialized.has_value());
                        REQUIRE(deserialized.value() != nullptr);
                        CHECK(deserialized.value()->getType() == chat::messages::Message::Type::Request);

                        decltype(message)* casted = nullptr;
                        REQUIRE_NOTHROW(casted = dynamic_cast<decltype(message)*>(deserialized.value().get()));
                        CHECK(casted->getType() == chat::messages::Request::Type::Ping);
                    }
                }
            }
        }

        AND_GIVEN("A pong response message")
        {
            chat::messages::Pong message;

            WHEN("The message is serialized using the serializer")
            {
                auto serialized = serializer.serialize(message);

                AND_WHEN("The message is deserialized using the serializer")
                {
                    auto deserialized = serializer.deserialize(serialized);

                    THEN("The deserialized message is the same as the original message")
                    {
                        REQUIRE(deserialized.has_value());
                        REQUIRE(deserialized.value() != nullptr);
                        CHECK(deserialized.value()->getType() == chat::messages::Message::Type::Response);

                        decltype(message)* casted = nullptr;
                        REQUIRE_NOTHROW(casted = dynamic_cast<decltype(message)*>(deserialized.value().get()));
                        CHECK(casted->getType() == chat::messages::Response::Type::Pong);
                    }
                }
            }
        }
    }
}
