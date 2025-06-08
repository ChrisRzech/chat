#include "chat/common/FixedBuffer.hpp"
#include "chat/common/utility.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <cstdint>

TEMPLATE_TEST_CASE("Converting to and from network byte order", "[byte order]",
                   std::int8_t, std::uint8_t, std::int16_t, std::uint16_t,
                   std::int32_t, std::uint32_t, std::int64_t, std::uint64_t)
{
    // Testing byte order conversion is difficult since the machine usually
    // can't switch the native byte ordering. Some architectures, like ARM
    // (https://stackoverflow.com/a/28726501/21445636), can change byte ordering
    // in the middle of execution. However, that assumption will not be used in
    // testing here. Instead, it will be assumed that testing on a machine
    // without such functionality is sufficient.

    auto getExpectedNetworkBytes = [](auto value) constexpr {
        chat::common::FixedBuffer<sizeof(value)> bytes = {};
        for(std::size_t i = 0; i < bytes.size(); i++) {
            bytes[i] =
                static_cast<std::byte>(i == bytes.size() - 1 ? value : 0);
        }
        return bytes;
    };

    using Integral = TestType;
    constexpr Integral value = 42;
    constexpr auto expectedNetworkBytes = getExpectedNetworkBytes(value);

    constexpr auto networkBytes =
        chat::common::utility::toNetworkByteOrder(value);
    REQUIRE(networkBytes == expectedNetworkBytes);

    constexpr auto reconstructedValue =
        chat::common::utility::toHostByteOrder<Integral>(networkBytes);
    REQUIRE(reconstructedValue == value);
}
