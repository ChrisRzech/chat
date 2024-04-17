#include "chat/common/OutputByteStream.hpp"
#include "chat/common/utility.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <array>
#include <cstddef>

#include <iostream>

SCENARIO("Writing data into an output byte stream", "[OutputByteStream]")
{
    GIVEN("An output byte stream")
    {
        chat::common::OutputByteStream stream;

        AND_GIVEN("Data")
        {
            constexpr std::size_t DATA_SIZE = 256;
            std::array<std::byte, DATA_SIZE> data;
            std::generate(data.begin(), data.end(), [i = 0]() mutable {return static_cast<std::byte>(i++);});

            WHEN("The data is written to the stream")
            {
                stream.write(data.data(), data.size());

                THEN("The stream contains the data")
                {
                    REQUIRE(stream.getData().size() == data.size());
                    CHECK(std::equal(stream.getData().begin(), stream.getData().end(), data.begin()));
                }
            }
        }
    }
}

SCENARIO("Inserting a byte array into an output byte stream", "[OutputByteStream]")
{
    GIVEN("An output byte stream")
    {
        chat::common::OutputByteStream stream;

        AND_GIVEN("A byte array")
        {
            constexpr std::size_t DATA_SIZE = 256;
            chat::common::ByteArray<DATA_SIZE> data;
            std::generate(data.begin(), data.end(), [i = 0]() mutable {return static_cast<std::byte>(i++);});

            WHEN("The byte array is written to the stream")
            {
                stream << data;

                THEN("The stream contains the data of the byte array")
                {
                    REQUIRE(stream.getData().size() == data.size());
                    CHECK(std::equal(stream.getData().begin(), stream.getData().end(), data.begin()));
                }
            }
        }
    }
}

TEMPLATE_TEST_CASE("Inserting an integral into an output byte stream", "[OutputByteStream]", std::int8_t, std::uint8_t, std::int16_t,
    std::uint16_t, std::int32_t, std::uint32_t, std::int64_t, std::uint64_t)
{
    GIVEN("An output byte stream")
    {
        chat::common::OutputByteStream stream;

        AND_GIVEN("An integral")
        {
            using Integral = TestType;
            constexpr Integral value = 42;

            WHEN("The integral is written to the stream")
            {
                stream << value;

                THEN("The stream contains the integral in network byte order")
                {
                    constexpr auto expectedData = chat::common::utility::toNetworkByteOrder(value);

                    REQUIRE(stream.getData().size() == expectedData.size());
                    CHECK(std::equal(stream.getData().begin(), stream.getData().end(), expectedData.begin()));
                }
            }
        }
    }
}

SCENARIO("Inserting a byte span into an output byte stream", "[OutputByteStream]")
{
    GIVEN("An output byte stream")
    {
        chat::common::OutputByteStream stream;

        AND_GIVEN("Data and byte span of the data")
        {
            constexpr std::size_t DATA_SIZE = 256;
            std::array<std::byte, DATA_SIZE> data;
            std::generate(data.begin(), data.end(), [i = 0]() mutable {return static_cast<std::byte>(i++);});
            chat::common::ByteSpan span{data.data(), data.size()};

            WHEN("The byte span is written to the stream")
            {
                stream << span;

                THEN("The stream contains the size of the byte span and data of the byte span")
                {
                    std::array<std::byte, sizeof(std::uint32_t) + DATA_SIZE> expectedData;
                    constexpr auto sizeBytes = chat::common::utility::toNetworkByteOrder<std::uint32_t>(DATA_SIZE);
                    auto expectedDataEnd = std::copy(sizeBytes.begin(), sizeBytes.end(), expectedData.begin());
                    std::copy(data.begin(), data.end(), expectedDataEnd);

                    REQUIRE(stream.getData().size() == expectedData.size());
                    CHECK(std::equal(stream.getData().begin(), stream.getData().end(), expectedData.begin()));
                }
            }
        }
    }
}

SCENARIO("Inserting a byte string into an output byte stream", "[OutputByteStream]")
{
    GIVEN("An output byte stream")
    {
        chat::common::OutputByteStream stream;

        AND_GIVEN("A byte string")
        {
            constexpr std::size_t DATA_SIZE = 256;
            chat::common::ByteString data;
            data.resize(DATA_SIZE);
            std::generate(data.begin(), data.end(), [i = 0]() mutable {return static_cast<std::byte>(i++);});

            WHEN("The byte string is written to the stream")
            {
                stream << data;

                THEN("The stream contains the size of the byte string and data of the byte string")
                {
                    std::array<std::byte, sizeof(std::uint32_t) + DATA_SIZE> expectedData;
                    constexpr auto sizeBytes = chat::common::utility::toNetworkByteOrder<std::uint32_t>(DATA_SIZE);
                    auto expectedDataEnd = std::copy(sizeBytes.begin(), sizeBytes.end(), expectedData.begin());
                    std::copy(data.begin(), data.end(), expectedDataEnd);

                    REQUIRE(stream.getData().size() == expectedData.size());
                    CHECK(std::equal(stream.getData().begin(), stream.getData().end(), expectedData.begin()));
                }
            }
        }
    }
}
