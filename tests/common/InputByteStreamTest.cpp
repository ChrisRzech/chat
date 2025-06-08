#include "chat/common/Buffer.hpp"
#include "chat/common/BufferView.hpp"
#include "chat/common/FixedBuffer.hpp"
#include "chat/common/InputByteStream.hpp"
#include "chat/common/utility.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

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

TEST_CASE("Empty stream is initially in a good state", "[InputByteStream]")
{
    const chat::common::InputByteStream stream{chat::common::BufferView{}};
    REQUIRE(stream.isGood());
    REQUIRE(stream.isEmpty());
}

TEST_CASE("Non-empty stream is initially in a good state", "[InputByteStream]")
{
    constexpr auto bytes = createBytes();
    const chat::common::InputByteStream stream{
        chat::common::BufferView{bytes.data(), bytes.size()}};
    REQUIRE(stream.isGood());
    REQUIRE(!stream.isEmpty());
}

TEST_CASE("Reading from an empty stream", "[InputByteStream]")
{
    chat::common::InputByteStream stream{chat::common::BufferView{}};
    auto read = stream.read(1);
    REQUIRE(!stream.isGood());
    REQUIRE(!read.has_value());
}

TEST_CASE("Reading from a non-empty stream", "[InputByteStream]")
{
    constexpr auto bytes = createBytes();
    const chat::common::BufferView view{bytes.data(), bytes.size()};
    chat::common::InputByteStream stream{view};

    auto read = stream.read(bytes.size());
    REQUIRE(stream.isGood());
    REQUIRE(stream.isEmpty());
    REQUIRE(read.has_value());
    REQUIRE(read.value().size() == bytes.size());
    REQUIRE(std::equal(read.value().begin(), read.value().end(), view.begin()));
}

TEST_CASE("Reading more than there is from a stream", "[InputByteStream]")
{
    constexpr auto bytes = createBytes();
    chat::common::InputByteStream stream{
        chat::common::BufferView{bytes.data(), bytes.size()}};

    auto read = stream.read(bytes.size() + 1);
    REQUIRE(!stream.isGood());
    REQUIRE(!stream.isEmpty());
    REQUIRE(!read.has_value());
}

TEST_CASE("Reading a byte array from a stream", "[InputByteStream]")
{
    constexpr auto bytes = createBytes();
    chat::common::InputByteStream stream{
        chat::common::BufferView{bytes.data(), bytes.size()}};

    chat::common::FixedBuffer<bytes.size()> array = {};
    stream >> array;
    REQUIRE(stream.isGood());
    REQUIRE(stream.isEmpty());
    REQUIRE(array == bytes);
}

TEMPLATE_TEST_CASE("Reading an integral from a stream", "[InputByteStream]",
                   std::int8_t, std::uint8_t, std::int16_t, std::uint16_t,
                   std::int32_t, std::uint32_t, std::int64_t, std::uint64_t)
{
    using Integral = TestType;
    constexpr Integral expected = 42;
    constexpr auto expectedBytes =
        chat::common::utility::toNetworkByteOrder(expected);

    chat::common::InputByteStream stream{
        chat::common::BufferView{expectedBytes.data(), expectedBytes.size()}};

    Integral value;
    stream >> value;
    REQUIRE(stream.isGood());
    REQUIRE(stream.isEmpty());
    REQUIRE(value == expected);
}

TEST_CASE("Reading a byte view from a stream", "[InputByteStream]")
{
    constexpr auto bytes = createBytes();
    constexpr auto sizedBytes = createSizedBytes(bytes);

    chat::common::InputByteStream stream{
        chat::common::BufferView{sizedBytes.data(), sizedBytes.size()}};

    chat::common::BufferView view;
    stream >> view;
    REQUIRE(stream.isGood());
    REQUIRE(stream.isEmpty());

    const chat::common::BufferView expected{bytes.data(), bytes.size()};
    REQUIRE(view.size() == expected.size());
    REQUIRE(std::equal(view.begin(), view.end(), expected.begin()));
}

TEST_CASE("Reading a buffer from a stream", "[InputByteStream]")
{
    constexpr auto bytes = createBytes();
    constexpr auto sizedBytes = createSizedBytes(bytes);

    chat::common::InputByteStream stream{
        chat::common::BufferView{sizedBytes.data(), sizedBytes.size()}};

    chat::common::Buffer buffer;
    stream >> buffer;
    REQUIRE(stream.isGood());
    REQUIRE(stream.isEmpty());

    const chat::common::Buffer expected{bytes.begin(), bytes.end()};
    REQUIRE(buffer == expected);
}

TEST_CASE("An empty InputByteStream has no readable bytes", "[InputByteStream]")
{
    const chat::common::InputByteStream stream{chat::common::BufferView{}};
    REQUIRE(stream.isEmpty());
    REQUIRE(stream.getReadableCount() == 0);
}

TEST_CASE("A non-empty InputByteStream has readable bytes", "[InputByteStream]")
{
    constexpr auto bytes = createBytes();
    const chat::common::BufferView view{bytes.data(), bytes.size()};
    const chat::common::InputByteStream stream{view};
    REQUIRE(!stream.isEmpty());
    REQUIRE(stream.getReadableCount() == bytes.size());
}

TEST_CASE(
    "Reading from a non-empty InputByteStream changes the number of readable "
    "bytes left",
    "[InputByteStream]")
{
    constexpr auto bytes = createBytes();
    constexpr auto readSize = bytes.size() - 1;
    constexpr auto expectedReadableCount = bytes.size() - readSize;
    const chat::common::BufferView view{bytes.data(), bytes.size()};
    chat::common::InputByteStream stream{view};
    auto read = stream.read(readSize);
    REQUIRE(read.has_value());
    REQUIRE(stream.getReadableCount() == expectedReadableCount);
}

TEST_CASE(
    "Failing to read from a non-empty InputByteStream does not change the "
    "number of readable bytes left",
    "[InputByteStream]")
{
    constexpr auto bytes = createBytes();
    constexpr auto readSize = bytes.size() + 1;
    constexpr auto expectedReadableCount = bytes.size();
    const chat::common::BufferView view{bytes.data(), bytes.size()};
    chat::common::InputByteStream stream{view};
    auto read = stream.read(readSize);
    REQUIRE(!read.has_value());
    REQUIRE(stream.getReadableCount() == expectedReadableCount);
}
