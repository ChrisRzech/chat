#include "chat/common/OutputByteStream.hpp"
#include "chat/common/utility.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <array>
#include <cstddef>

namespace
{
constexpr auto createBytes()
{
    constexpr std::size_t size = 256;
    std::array<std::byte, size> bytes = {};
    int i = 0;
    for(auto& byte : bytes) {
        byte = static_cast<std::byte>(i++);
    }
    return bytes;
}

template<std::size_t size>
constexpr auto createSizedBytes(const std::array<std::byte, size>& bytes)
{
    auto copy = [](auto begin, auto end, auto result) {
        for(auto curr = begin; curr != end; curr++) {
            *result = *curr;
            result++;
        }
        return result;
    };

    std::array<std::byte, sizeof(std::uint32_t) + size> sizedBytes = {};
    constexpr auto sizeBytes =
        chat::common::utility::toNetworkByteOrder<std::uint32_t>(size);
    auto end = copy(sizeBytes.begin(), sizeBytes.end(), sizedBytes.begin());
    copy(bytes.begin(), bytes.end(), end);
    return sizedBytes;
}
}

TEST_CASE("Stream is initially empty", "[OutputByteStream]")
{
    const chat::common::OutputByteStream stream;
    REQUIRE(stream.getData().empty());
}

TEST_CASE("Writing into a stream", "[OutputByteStream]")
{
    constexpr auto bytes = createBytes();
    const chat::common::ByteSpan span{bytes.data(), bytes.size()};

    chat::common::OutputByteStream stream;
    stream.write(span);
    REQUIRE(stream.getData().size() == bytes.size());
    REQUIRE(std::equal(stream.getData().begin(), stream.getData().end(),
                       bytes.begin()));
}

TEST_CASE("Writing a byte array into a stream", "[OutputByteStream]")
{
    constexpr auto bytes = createBytes();

    chat::common::OutputByteStream stream;
    stream << bytes;
    REQUIRE(stream.getData().size() == bytes.size());
    REQUIRE(std::equal(stream.getData().begin(), stream.getData().end(),
                       bytes.begin()));
}

TEMPLATE_TEST_CASE("Writing an integral into a stream", "[OutputByteStream]",
                   std::int8_t, std::uint8_t, std::int16_t, std::uint16_t,
                   std::int32_t, std::uint32_t, std::int64_t, std::uint64_t)
{
    using Integral = TestType;
    constexpr Integral value = 42;

    chat::common::OutputByteStream stream;
    stream << value;

    constexpr auto expected = chat::common::utility::toNetworkByteOrder(value);
    REQUIRE(stream.getData().size() == expected.size());
    REQUIRE(std::equal(stream.getData().begin(), stream.getData().end(),
                       expected.begin()));
}

TEST_CASE("Writing a byte span into a stream", "[OutputByteStream]")
{
    constexpr auto bytes = createBytes();
    const chat::common::ByteSpan span{bytes.data(), bytes.size()};

    chat::common::OutputByteStream stream;
    stream << span;

    constexpr auto expected = createSizedBytes(bytes);
    REQUIRE(stream.getData().size() == expected.size());
    REQUIRE(std::equal(stream.getData().begin(), stream.getData().end(),
                       expected.begin()));
}

TEST_CASE("Writing a byte string into a stream", "[OutputByteStream]")
{
    constexpr auto bytes = createBytes();
    const chat::common::ByteString byteString{bytes.begin(), bytes.end()};

    chat::common::OutputByteStream stream;
    stream << byteString;

    constexpr auto expected = createSizedBytes(bytes);
    REQUIRE(stream.getData().size() == expected.size());
    REQUIRE(std::equal(stream.getData().begin(), stream.getData().end(),
                       expected.begin()));
}
