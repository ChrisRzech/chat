#include "chat/messages/Close.hpp"
#include "chat/messages/serialize.hpp"

#include "chat/messages/request/Ping.hpp"

#include "chat/messages/response/Pong.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("Using the serializer to serializing and deserializing messages",
         "[serialize]")
{
    AND_GIVEN("A close message")
    {
        const chat::messages::Close message;

        WHEN("The message is serialized")
        {
            auto serialized = chat::messages::serialize(message);

            AND_WHEN("The message is deserialized")
            {
                chat::common::ByteSpan bytes{serialized.data(),
                                             serialized.size()};
                auto deserialized = chat::messages::deserialize(bytes);

                THEN(
                    "The deserialized message is the same as the original "
                    "message")
                {
                    REQUIRE(deserialized.has_value());
                    REQUIRE(deserialized.value() != nullptr);
                    CHECK(deserialized.value()->getType() ==
                          chat::messages::Message::Type::Close);
                }
            }
        }
    }

    AND_GIVEN("A ping request message")
    {
        const chat::messages::Ping message;

        WHEN("The message is serialized")
        {
            auto serialized = chat::messages::serialize(message);

            AND_WHEN("The message is deserialized")
            {
                chat::common::ByteSpan bytes{serialized.data(),
                                             serialized.size()};
                auto deserialized = chat::messages::deserialize(bytes);

                THEN(
                    "The deserialized message is the same as the original "
                    "message")
                {
                    REQUIRE(deserialized.has_value());
                    REQUIRE(deserialized.value() != nullptr);
                    CHECK(deserialized.value()->getType() ==
                          chat::messages::Message::Type::Request);

                    auto casted = dynamic_cast<decltype(message)*>(
                        deserialized.value().get());
                    REQUIRE(casted != nullptr);
                    CHECK(casted->getType() ==
                          chat::messages::Request::Type::Ping);
                }
            }
        }
    }

    AND_GIVEN("A pong response message")
    {
        const chat::messages::Pong message;

        WHEN("The message is serialized")
        {
            auto serialized = chat::messages::serialize(message);

            AND_WHEN("The message is deserialized")
            {
                chat::common::ByteSpan bytes{serialized.data(),
                                             serialized.size()};
                auto deserialized = chat::messages::deserialize(bytes);

                THEN(
                    "The deserialized message is the same as the original "
                    "message")
                {
                    REQUIRE(deserialized.has_value());
                    REQUIRE(deserialized.value() != nullptr);
                    CHECK(deserialized.value()->getType() ==
                          chat::messages::Message::Type::Response);

                    auto casted = dynamic_cast<decltype(message)*>(
                        deserialized.value().get());
                    REQUIRE(casted != nullptr);
                    CHECK(casted->getType() ==
                          chat::messages::Response::Type::Pong);
                }
            }
        }
    }
}
