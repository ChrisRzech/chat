#include "chat/common/ByteArray.hpp"
#include "chat/common/ByteSpan.hpp"
#include "chat/common/ByteString.hpp"
#include "chat/common/InputByteStream.hpp"
#include "chat/common/utility.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <algorithm>

SCENARIO("Reading data from an input byte stream", "[InputByteStream]")
{
    GIVEN("An input byte stream containing data")
    {
        constexpr std::size_t DATA_SIZE = 256;
        std::array<std::byte, DATA_SIZE> data;
        std::generate(data.begin(), data.end(), [i = 0]() mutable {return static_cast<std::byte>(i++);});
        chat::common::InputByteStream stream{chat::common::ByteSpan{data.data(), data.size()}};

        THEN("The stream is in a good state")
        {
            CHECK(stream.isGood());
        }

        THEN("The stream is not empty")
        {
            CHECK(!stream.isEmpty());
        }

        WHEN("Reading all the data from the stream")
        {
            auto read = stream.read(DATA_SIZE);

            THEN("The read operation was successful")
            {
                CHECK(stream.isGood());
            }

            THEN("The stream is empty")
            {
                CHECK(stream.isEmpty());
            }

            THEN("The returned data matches the data used for the stream")
            {
                REQUIRE(read.has_value());
                REQUIRE(read.value().getSize() == data.size());
                CHECK(std::equal(read.value().getData(), read.value().getData() + read.value().getSize(), data.begin()));
            }

            WHEN("Attempting to read more data than there is in the stream")
            {
                auto badRead = stream.read(DATA_SIZE);

                THEN("The read operation was unsuccessful")
                {
                    CHECK(!stream.isGood());
                }

                THEN("The result has no value")
                {
                    CHECK(!badRead.has_value());
                }
            }
        }
    }
}

SCENARIO("Reading a byte array from an input byte stream", "[InputByteStream]")
{
    GIVEN("An input byte stream containing data")
    {
        constexpr std::size_t DATA_SIZE = 256;
        std::array<std::byte, DATA_SIZE> data;
        std::generate(data.begin(), data.end(), [i = 0]() mutable {return static_cast<std::byte>(i++);});
        chat::common::InputByteStream stream{chat::common::ByteSpan{data.data(), data.size()}};

        THEN("The stream is in a good state")
        {
            CHECK(stream.isGood());
        }

        THEN("The stream is not empty")
        {
            CHECK(!stream.isEmpty());
        }

        WHEN("Reading all the data from the stream into the byte array")
        {
            chat::common::ByteArray<DATA_SIZE> inputData = {};
            stream >> inputData;

            THEN("The read operation was successful")
            {
                CHECK(stream.isGood());
            }

            THEN("The stream is empty")
            {
                CHECK(stream.isEmpty());
            }

            THEN("The byte array contains the data")
            {
                REQUIRE(inputData.size() == data.size());
                CHECK(std::equal(inputData.begin(), inputData.end(), data.begin()));
            }
        }
    }
}

TEMPLATE_TEST_CASE("Reading an integral from an input byte stream", "[InputByteStream]", std::int8_t, std::uint8_t, std::int16_t,
    std::uint16_t, std::int32_t, std::uint32_t, std::int64_t, std::uint64_t)
{
    GIVEN("An input byte stream containing an integral")
    {
        using Integral = TestType;
        constexpr Integral expectedValue = 42;
        constexpr auto expectedValueBytes = chat::common::utility::toNetworkByteOrder(expectedValue);
        chat::common::InputByteStream stream{chat::common::ByteSpan{expectedValueBytes.data(), expectedValueBytes.size()}};

        THEN("The stream is in a good state")
        {
            CHECK(stream.isGood());
        }

        THEN("The stream is not empty")
        {
            CHECK(!stream.isEmpty());
        }

        WHEN("Reading all the data from the stream into an integral")
        {
            Integral value;
            stream >> value;

            THEN("The read operation was successful")
            {
                CHECK(stream.isGood());
            }

            THEN("The stream is empty")
            {
                CHECK(stream.isEmpty());
            }

            THEN("The integral contains the expected value")
            {
                CHECK(value == expectedValue);
            }
        }
    }
}

SCENARIO("Reading a byte span from an input byte stream", "[InputByteStream]")
{
    GIVEN("An input byte stream containing a byte span")
    {
        constexpr std::size_t SIZE_SIZE = sizeof(std::uint32_t);
        constexpr std::size_t DATA_SIZE = 256;
        std::array<std::byte, SIZE_SIZE + DATA_SIZE> data;
        constexpr auto sizeBytes = chat::common::utility::toNetworkByteOrder<std::uint32_t>(DATA_SIZE);
        auto dataStart = std::copy(sizeBytes.begin(), sizeBytes.end(), data.begin());
        std::generate(dataStart, data.end(), [i = 0]() mutable {return static_cast<std::byte>(i++);});
        chat::common::ByteSpan expectedSpan{dataStart, DATA_SIZE};

        chat::common::InputByteStream stream{chat::common::ByteSpan{data.data(), data.size()}};

        THEN("The stream is in a good state")
        {
            CHECK(stream.isGood());
        }

        THEN("The stream is not empty")
        {
            CHECK(!stream.isEmpty());
        }

        WHEN("Reading all the data from the stream into a byte stream")
        {
            chat::common::ByteSpan span;
            stream >> span;

            THEN("The read operation was successful")
            {
                CHECK(stream.isGood());
            }

            THEN("The stream is empty")
            {
                CHECK(stream.isEmpty());
            }

            THEN("The byte span contains the expected value")
            {
                REQUIRE(span.getSize() == expectedSpan.getSize());
                CHECK(std::equal(span.getData(), span.getData() + span.getSize(), expectedSpan.getData()));
            }
        }
    }
}

SCENARIO("Reading a byte string from an input byte stream", "[InputByteStream]")
{
    GIVEN("An input byte stream containing a byte string")
    {
        constexpr std::size_t SIZE_SIZE = sizeof(std::uint32_t);
        constexpr std::size_t DATA_SIZE = 256;
        std::array<std::byte, SIZE_SIZE + DATA_SIZE> data;
        constexpr auto sizeBytes = chat::common::utility::toNetworkByteOrder<std::uint32_t>(DATA_SIZE);
        auto dataStart = std::copy(sizeBytes.begin(), sizeBytes.end(), data.begin());
        std::generate(dataStart, data.end(), [i = 0]() mutable {return static_cast<std::byte>(i++);});
        chat::common::ByteString expectedString{dataStart, data.end()};

        chat::common::InputByteStream stream{chat::common::ByteSpan{data.data(), data.size()}};

        THEN("The stream is in a good state")
        {
            CHECK(stream.isGood());
        }

        THEN("The stream is not empty")
        {
            CHECK(!stream.isEmpty());
        }

        WHEN("Reading all the data from the stream into a byte stream")
        {
            chat::common::ByteString string;
            stream >> string;

            THEN("The read operation was successful")
            {
                CHECK(stream.isGood());
            }

            THEN("The stream is empty")
            {
                CHECK(stream.isEmpty());
            }

            THEN("The byte string contains the expected value")
            {
                REQUIRE(string.size() == expectedString.size());
                CHECK(std::equal(string.begin(), string.end(), expectedString.begin()));
            }
        }
    }
}
