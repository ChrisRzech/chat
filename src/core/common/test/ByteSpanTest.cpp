#include "chat/common/ByteSpan.hpp"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <array>
#include <cstddef>

namespace
{
constexpr auto createBytes()
{
    constexpr std::size_t size = 10;
    std::array<std::byte, size> bytes = {};
    int i = 0;
    for(auto& byte : bytes) {
        byte = static_cast<std::byte>(i++);
    }
    return bytes;
}
}

TEST_CASE("Creating an empty span", "[ByteSpan]")
{
    const chat::common::ByteSpan span;
    REQUIRE(span.getData() == nullptr);
    REQUIRE(span.getSize() == 0);
}

TEST_CASE("Creating a span over a array", "[ByteSpan]")
{
    constexpr auto bytes = createBytes();
    const chat::common::ByteSpan span{bytes.data(), bytes.size()};
    REQUIRE(span.getData() == bytes.data());
    REQUIRE(span.getSize() == bytes.size());
    REQUIRE(std::equal(span.begin(), span.end(), bytes.begin()));
}

TEST_CASE("Using span in range-based for loop", "[ByteSpan]")
{
    constexpr auto bytes = createBytes();
    const chat::common::ByteSpan span{bytes.data(), bytes.size()};
    int expected = 0;
    for(auto byte : span) {
        REQUIRE(byte == static_cast<std::byte>(expected));
        expected++;
    }
}

TEST_CASE("Comparing equal spans", "[ByteSpan]")
{
    constexpr auto bytes = createBytes();
    const chat::common::ByteSpan span1{bytes.data(), bytes.size()};
    const chat::common::ByteSpan span2{bytes.data(), bytes.size()};
    REQUIRE(span1 == span2);
}

TEST_CASE("Comparing spans not equal by size", "[ByteSpan]")
{
    constexpr auto bytes = createBytes();
    const chat::common::ByteSpan span1{bytes.data(), bytes.size()};
    const chat::common::ByteSpan span2{bytes.data(), bytes.size() - 1};
    REQUIRE(span1 != span2);
}

TEST_CASE("Comparing spans not equal by data", "[ByteSpan]")
{
    auto bytes1 = createBytes();
    const chat::common::ByteSpan span1{bytes1.data(), bytes1.size()};

    auto bytes2 = createBytes();
    bytes2[bytes2.size() - 1] = std::byte{0};
    const chat::common::ByteSpan span2{bytes2.data(), bytes2.size()};

    REQUIRE(span1 != span2);
}
