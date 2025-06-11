#include "chat/common/EnumMeta.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstdint>
#include <string_view>
#include <type_traits>

TEMPLATE_TEST_CASE("Get an array of all enum values", "[EnumMeta]", std::int8_t,
                   std::uint8_t, std::int16_t, std::uint16_t, std::int32_t,
                   std::uint32_t, std::int64_t, std::uint64_t)
{
    enum class Enum : TestType // NOLINT(performance-enum-size)
    {
        One,
        Two,
        Three
    };

    constexpr auto expected = std::array{Enum::One, Enum::Two, Enum::Three};
    REQUIRE(chat::common::enummeta::getValues<Enum>() == expected);
}

TEMPLATE_TEST_CASE("Get a mapping of enum values to names", "[EnumMeta]",
                   std::int8_t, std::uint8_t, std::int16_t, std::uint16_t,
                   std::int32_t, std::uint32_t, std::int64_t, std::uint64_t)
{
    enum class Enum : TestType // NOLINT(performance-enum-size)
    {
        One,
        Two,
        Three
    };

    constexpr auto expected = std::array{
        std::make_pair(Enum::One, std::string_view{"One"}),
        std::make_pair(Enum::Two, std::string_view{"Two"}),
        std::make_pair(Enum::Three, std::string_view{"Three"}),
    };
    REQUIRE(chat::common::enummeta::getNames<Enum>() == expected);
}

TEMPLATE_TEST_CASE("Get the name of the enum value", "[EnumMeta]", std::int8_t,
                   std::uint8_t, std::int16_t, std::uint16_t, std::int32_t,
                   std::uint32_t, std::int64_t, std::uint64_t)
{
    enum class Enum : TestType // NOLINT(performance-enum-size)
    {
        One,
        Two,
        Three
    };

    REQUIRE(chat::common::enummeta::toName(Enum::One).value() == "One");
    REQUIRE(chat::common::enummeta::toName(Enum::Two).value() == "Two");
    REQUIRE(chat::common::enummeta::toName(Enum::Three).value() == "Three");
    REQUIRE(!chat::common::enummeta::toName(static_cast<Enum>(10)).has_value());
}

TEMPLATE_TEST_CASE("Get the enum value of the name", "[EnumMeta]", std::int8_t,
                   std::uint8_t, std::int16_t, std::uint16_t, std::int32_t,
                   std::uint32_t, std::int64_t, std::uint64_t)
{
    enum class Enum : TestType // NOLINT(performance-enum-size)
    {
        One,
        Two,
        Three
    };

    REQUIRE(chat::common::enummeta::fromName<Enum>("One").value() == Enum::One);
    REQUIRE(chat::common::enummeta::fromName<Enum>("Two").value() == Enum::Two);
    REQUIRE(chat::common::enummeta::fromName<Enum>("Three").value() ==
            Enum::Three);
    REQUIRE(!chat::common::enummeta::fromName<Enum>("invalid").has_value());
}

TEMPLATE_TEST_CASE("Get the underlying integer value of the enum value",
                   "[EnumMeta]", std::int8_t, std::uint8_t, std::int16_t,
                   std::uint16_t, std::int32_t, std::uint32_t, std::int64_t,
                   std::uint64_t)
{
    enum class Enum : TestType // NOLINT(performance-enum-size)
    {
        One,
        Two,
        Three
    };

    REQUIRE(chat::common::enummeta::toUnderlying(Enum::One) ==
            static_cast<std::underlying_type_t<Enum>>(Enum::One));
    REQUIRE(chat::common::enummeta::toUnderlying(Enum::Two) ==
            static_cast<std::underlying_type_t<Enum>>(Enum::Two));
    REQUIRE(chat::common::enummeta::toUnderlying(Enum::Three) ==
            static_cast<std::underlying_type_t<Enum>>(Enum::Three));
}

TEMPLATE_TEST_CASE("Get the enum value of the underlying value", "[EnumMeta]",
                   std::int8_t, std::uint8_t, std::int16_t, std::uint16_t,
                   std::int32_t, std::uint32_t, std::int64_t, std::uint64_t)
{
    enum class Enum : TestType // NOLINT(performance-enum-size)
    {
        One,
        Two,
        Three
    };

    REQUIRE(chat::common::enummeta::fromUnderlying<Enum>(
                static_cast<std::underlying_type_t<Enum>>(Enum::One))
                .value() == Enum::One);
    REQUIRE(chat::common::enummeta::fromUnderlying<Enum>(
                static_cast<std::underlying_type_t<Enum>>(Enum::Two))
                .value() == Enum::Two);
    REQUIRE(chat::common::enummeta::fromUnderlying<Enum>(
                static_cast<std::underlying_type_t<Enum>>(Enum::Three))
                .value() == Enum::Three);
    REQUIRE(!chat::common::enummeta::fromUnderlying<Enum>(
                 static_cast<std::underlying_type_t<Enum>>(42))
                 .has_value());
}

namespace
{
enum class EnumRangeTest // NOLINT(performance-enum-size)
{
    BelowMin,
    Min,
    Good,
    Max,
    AboveMax
};

constexpr std::underlying_type_t<EnumRangeTest> toUnder(EnumRangeTest value)
{
    return static_cast<std::underlying_type_t<EnumRangeTest>>(value);
}
}

template<>
struct chat::common::enummeta::Range<EnumRangeTest>
{
    static constexpr auto min = toUnder(EnumRangeTest::Min);
    static constexpr auto max = toUnder(EnumRangeTest::Max);
};

TEST_CASE("Range determines which enum values are valid", "[EnumMeta]")
{
    REQUIRE(
        !chat::common::enummeta::toName(EnumRangeTest::BelowMin).has_value());
    REQUIRE(chat::common::enummeta::toName(EnumRangeTest::Min).value() ==
            "Min");
    REQUIRE(chat::common::enummeta::toName(EnumRangeTest::Good).value() ==
            "Good");
    REQUIRE(chat::common::enummeta::toName(EnumRangeTest::Max).value() ==
            "Max");
    REQUIRE(
        !chat::common::enummeta::toName(EnumRangeTest::AboveMax).has_value());

    REQUIRE(!chat::common::enummeta::fromName<EnumRangeTest>("BelowMin")
                 .has_value());
    REQUIRE(chat::common::enummeta::fromName<EnumRangeTest>("Min").value() ==
            EnumRangeTest::Min);
    REQUIRE(chat::common::enummeta::fromName<EnumRangeTest>("Good").value() ==
            EnumRangeTest::Good);
    REQUIRE(chat::common::enummeta::fromName<EnumRangeTest>("Max").value() ==
            EnumRangeTest::Max);
    REQUIRE(!chat::common::enummeta::fromName<EnumRangeTest>("AboveMax")
                 .has_value());

    REQUIRE(!chat::common::enummeta::fromUnderlying<EnumRangeTest>(
                 toUnder(EnumRangeTest::BelowMin))
                 .has_value());
    REQUIRE(chat::common::enummeta::fromUnderlying<EnumRangeTest>(
                toUnder(EnumRangeTest::Min))
                .value() == EnumRangeTest::Min);
    REQUIRE(chat::common::enummeta::fromUnderlying<EnumRangeTest>(
                toUnder(EnumRangeTest::Good))
                .value() == EnumRangeTest::Good);
    REQUIRE(chat::common::enummeta::fromUnderlying<EnumRangeTest>(
                toUnder(EnumRangeTest::Max))
                .value() == EnumRangeTest::Max);
    REQUIRE(!chat::common::enummeta::fromUnderlying<EnumRangeTest>(
                 toUnder(EnumRangeTest::AboveMax))
                 .has_value());
}
