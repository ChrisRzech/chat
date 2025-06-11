#include "chat/common/BufferView.hpp"
#include "chat/messages/IncrementalRequestDeserializer.hpp"
#include "chat/messages/request/Ping.hpp"
#include "chat/messages/serialize.hpp"

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstddef>

TEST_CASE("Incrementally deserializing a serialized request as a whole",
          "[IncrementalRequestDeserializer]")
{
    const chat::messages::Ping message;
    auto serialized = chat::messages::serialize(message);
    const chat::common::BufferView serializedView{serialized.data(),
                                                  serialized.size()};
    chat::messages::IncrementalRequestDeserializer deserializer;
    const auto result = deserializer.tryDeserialize(serializedView);
    REQUIRE(result.hasValue());
    REQUIRE(result.getValue()->getType() ==
            chat::messages::Request::Type::Ping);
}

TEST_CASE("Incrementally deserializing a serialized request in chunks",
          "[IncrementalRequestDeserializer]")
{
    using FailureReason =
        chat::messages::IncrementalRequestDeserializer::FailureReason;

    const chat::messages::Ping message;
    const auto serialized = chat::messages::serialize(message);
    const chat::common::BufferView serializedView{serialized.data(),
                                                  serialized.size()};
    chat::messages::IncrementalRequestDeserializer deserializer;

    std::size_t chunkOffset = 0;
    std::size_t chunkSize = 0;
    auto chunk = serializedView.subspan(chunkOffset, chunkSize);
    auto result = deserializer.tryDeserialize(chunk);
    REQUIRE(!result.hasValue());
    REQUIRE(result.getError() == FailureReason::Partial);

    chunkOffset += chunkSize;
    chunkSize = 1;
    chunk = serializedView.subspan(chunkOffset, chunkSize);
    result = deserializer.tryDeserialize(chunk);
    REQUIRE(!result.hasValue());
    REQUIRE(result.getError() == FailureReason::Partial);

    chunkOffset += chunkSize;
    chunkSize = 1;
    chunk = serializedView.subspan(chunkOffset, chunkSize);
    result = deserializer.tryDeserialize(chunk);
    REQUIRE(!result.hasValue());
    REQUIRE(result.getError() == FailureReason::Partial);

    chunkOffset += chunkSize;
    chunkSize = 2;
    chunk = serializedView.subspan(chunkOffset, chunkSize);
    result = deserializer.tryDeserialize(chunk);
    REQUIRE(!result.hasValue());
    REQUIRE(result.getError() == FailureReason::Partial);

    chunkOffset += chunkSize;
    chunkSize = 1;
    chunk = serializedView.subspan(chunkOffset, chunkSize);
    result = deserializer.tryDeserialize(chunk);
    REQUIRE(result.hasValue());
    REQUIRE(result.getValue()->getType() ==
            chat::messages::Request::Type::Ping);
}

TEST_CASE("Incrementally deserializing an invalid serialized request",
          "[IncrementalRequestDeserializer]")
{
    using FailureReason =
        chat::messages::IncrementalRequestDeserializer::FailureReason;

    // This should fail to serialize because 0xFF is not a valid request type ID
    constexpr auto data = std::array{
        std::byte{0x0}, std::byte{0x0},  std::byte{0x0},
        std::byte{0x1}, std::byte{0xFF},
    };
    const chat::common::BufferView view{data.data(), data.size()};

    chat::messages::IncrementalRequestDeserializer deserializer;
    const auto result = deserializer.tryDeserialize(view);
    REQUIRE(!result.hasValue());
    REQUIRE(result.getError() == FailureReason::Error);
}
