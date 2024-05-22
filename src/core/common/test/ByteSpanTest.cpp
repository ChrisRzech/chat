#include "chat/common/ByteSpan.hpp"

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstddef>

SCENARIO("Creating an empty byte span", "[ByteSpan]")
{
    GIVEN("An empty byte span")
    {
        const chat::common::ByteSpan span;

        THEN("The byte span does not refer to data")
        {
            CHECK(span.getData() == nullptr);
            CHECK(span.getSize() == 0);
        }
    }
}

SCENARIO("Creating a byte span over a byte array", "[ByteSpan]")
{
    GIVEN("A byte array")
    {
        constexpr std::array<std::byte, 10> bytes = {
            std::byte{0}, std::byte{1}, std::byte{2}, std::byte{3},
            std::byte{4}, std::byte{5}, std::byte{6}, std::byte{7},
            std::byte{8}, std::byte{9},
        };

        WHEN("A byte span is created over the byte array")
        {
            const chat::common::ByteSpan span{bytes.data(), bytes.size()};

            THEN("The span refers to the same bytes as the byte array")
            {
                REQUIRE(span.getData() == bytes.data());
                REQUIRE(span.getSize() == bytes.size());
                for(std::size_t i = 0; i < span.getSize(); i++) {
                    CHECK(span[i] == bytes.at(i));
                }
            }
        }
    }
}

SCENARIO("Using byte span in range-based for loop", "[ByteSpan]")
{
    GIVEN("A byte span")
    {
        constexpr std::array<std::byte, 10> bytes = {
            std::byte{0}, std::byte{1}, std::byte{2}, std::byte{3},
            std::byte{4}, std::byte{5}, std::byte{6}, std::byte{7},
            std::byte{8}, std::byte{9},
        };
        const chat::common::ByteSpan span{bytes.data(), bytes.size()};

        THEN("Using a ranged-based loop goes through all the elements")
        {
            int expectedValue = 0;
            for(auto byte : span) {
                CHECK(byte == static_cast<std::byte>(expectedValue));
                expectedValue++;
            }
        }
    }
}
