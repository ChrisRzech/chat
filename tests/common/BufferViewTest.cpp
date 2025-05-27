#include "chat/common/BufferView.hpp"

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

TEST_CASE("Creating an empty view", "[BufferView]")
{
    const chat::common::BufferView view;
    REQUIRE(view.getData() == nullptr);
    REQUIRE(view.getSize() == 0);
}

TEST_CASE("Creating a view over a array", "[BufferView]")
{
    constexpr auto bytes = createBytes();
    const chat::common::BufferView view{bytes.data(), bytes.size()};
    REQUIRE(view.getData() == bytes.data());
    REQUIRE(view.getSize() == bytes.size());
    REQUIRE(std::equal(view.begin(), view.end(), bytes.begin()));
}

TEST_CASE("Using view in range-based for loop", "[BufferView]")
{
    constexpr auto bytes = createBytes();
    const chat::common::BufferView view{bytes.data(), bytes.size()};
    int expected = 0;
    for(auto byte : view) {
        REQUIRE(byte == static_cast<std::byte>(expected));
        expected++;
    }
}

TEST_CASE("Comparing equal views", "[BufferView]")
{
    constexpr auto bytes = createBytes();
    const chat::common::BufferView view1{bytes.data(), bytes.size()};
    const chat::common::BufferView view2{bytes.data(), bytes.size()};
    REQUIRE(view1 == view2);
}

TEST_CASE("Comparing views not equal by size", "[BufferView]")
{
    constexpr auto bytes = createBytes();
    const chat::common::BufferView view1{bytes.data(), bytes.size()};
    const chat::common::BufferView view2{bytes.data(), bytes.size() - 1};
    REQUIRE(view1 != view2);
}

TEST_CASE("Comparing views not equal by data", "[BufferView]")
{
    auto bytes1 = createBytes();
    const chat::common::BufferView view1{bytes1.data(), bytes1.size()};

    auto bytes2 = createBytes();
    bytes2[bytes2.size() - 1] = std::byte{0};
    const chat::common::BufferView view2{bytes2.data(), bytes2.size()};

    REQUIRE(view1 != view2);
}
