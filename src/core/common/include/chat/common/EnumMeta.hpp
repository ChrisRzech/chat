#include <array>
#include <limits>
#include <optional>
#include <string_view>
#include <type_traits>

// Inspiration from:
// https://github.com/Neargye/magic_enum
// https://blog.rink.nu/2023/02/12/behind-the-magic-of-magic_enum/

namespace chat::common::enummeta
{
/**
 * @brief Defines the min and max value to check for when finding valid enum
 * values.
 *
 * @details This type defines the min and max of the range for a given enum
 * type. The range is used to find valid enum values. If an enum value falls
 * outside of the range, it won't be considered a value enum value.
 *
 * The default values are chosen since they should fit in any integer type.
 *
 * @tparam Enum The enum type.
 */
template<typename Enum>
struct Range
{
    static constexpr std::underlying_type_t<Enum> min =
        std::numeric_limits<unsigned char>::min();
    static constexpr std::underlying_type_t<Enum> max =
        std::numeric_limits<signed char>::max();
};

namespace detail
{
// The implementation is dependent on compiler specific implementations of the
// `__PRETTY_FUNCTION__` macro. If the compiler is not known, then the behavior
// should be unexpected. In addition
#if !defined(__GNUG__) && !defined(__clang__)
static_assert(false, "Compiler not supported with enummeta utility");
#endif

/**
 * @brief Constexpr-able string.
 *
 * @details The whole `__PRETTY_FUNCTION__` string is kept in memory since
 * `std::string_view`s to it are used. However, only the enum string is
 * necessary to hold. This type is intended to solve this by creating a copy of
 * the enum string in `__PRETTY_FUNCTION__`.
 *
 * This type is intended only for implementation purposes and not meant to be
 * used externally. To provide the data externally, an `std::string_view` of the
 * data can be obtained.
 *
 * @tparam size The size of the string.
 */
template<std::size_t size>
class ConstexprString
{
public:
    /**
     * @brief Construct a string.
     */
    constexpr explicit ConstexprString(std::string_view name)
      : data{}
    {
        for(std::size_t i = 0; i < size; i++) {
            data.at(i) = name.at(i);
        }
    }

    /**
     * @brief Check if the string is empty.
     *
     * @return True if the string is empty; otherwise, false.
     */
    constexpr bool isEmpty()
    {
        return size == 0;
    }

    /**
     * @brief Get a view of the string.
     *
     * @return A view of the string.
     */
    [[nodiscard]] constexpr std::string_view toStringView() const
    {
        return std::string_view{data.data(), size};
    }

private:
    std::array<char, size + 1> data;
};

/**
 * @brief Get the name of the enum value.
 *
 * @details Intended to be used in @c getName().
 *
 * When a non-type template parameter is passed to a function,
 * `__PRETTY_FUNCTION__` displays information about the parameter. This
 * information will allow us to:
 * 1. Validate an enum value
 * 2. Obtain a string of the enum value
 *
 * The following is the expectation of what `__PRETTY_FUNCTION__` should
 * contain and how to parse it. Below is what to expect from specific
 * compilers for valid and invalid cases.
 *
 * gcc:
 * "constexpr auto enummeta::detail::getNameHelper() [with Enum = Test; Enum
 * value = (Test)0]"
 * "constexpr auto enummeta::detail::getNameHelper() [with Enum = Test; Enum
 * value = Test::A]"
 *
 * clang:
 * "auto enummeta::detail::getNameHelper() [Enum = Test, value = (Test)0]"
 * "auto enummeta::detail::getNameHelper() [Enum = Test, value = Test::A]"
 *
 * Looking at the strings, it's not obvious how to parse this, but one
 * notable pattern is the difference on how a valid enum value is
 * represented from an invalid enum value. More specifically, a valid enum
 * value has a `:` right before it while an invalid enum value has a `)`
 * right before it. In addition, there is a bracket at the end of the
 * string, which can be easily removed.
 *
 * The function signature also plays a role in allowing
 * `__PRETTY_FUNCTION__` to be easily parsed this way. For example, putting an
 * explicit return type will add more information to the end of
 * `__PRETTY_FUNCTION__`.
 *
 * @tparam Enum The enum type.
 *
 * @tparam value The enum value.
 *
 * @return The name of the enum value.
 */
template<typename Enum, Enum value>
constexpr auto getNameHelper()
{
    static_assert(std::is_enum_v<Enum>);
    std::string_view name = __PRETTY_FUNCTION__;
    if(name.at(name.size() - 1) == ']') { // Expect a bracket at the end
        name.remove_suffix(1);

        auto valueStart = name.find_last_of(":)"); // Find happy spot :)
        if(name.at(valueStart) == ':') {
            name.remove_prefix(valueStart + 1);
        } else {
            name = "";
        }
    } else {
        name = "";
    }
    return name;
}

/**
 * @brief Get the name of the enum value.
 *
 * @tparam Enum The enum type.
 *
 * @tparam value The enum value.
 *
 * @return The name of the enum value.
 */
template<typename Enum, Enum value>
constexpr auto getName()
{
    constexpr auto name = getNameHelper<Enum, value>();
    return ConstexprString<name.size()>{name};
}

/**
 * @brief The name of the enum value.
 *
 * @tparam Enum The enum type.
 *
 * @tparam value The enum value.
 */
template<typename Enum, Enum value>
inline constexpr auto name = getName<Enum, value>();

/**
 * @brief Check if the value is a valid enum value.
 *
 * @tparam Enum The enum type.
 *
 * @tparam value The enum value.
 *
 * @return True if the value is a valid enum value; otherwise, false.
 */
template<typename Enum, std::underlying_type_t<Enum> value>
constexpr auto isValid()
{
    return !getName<Enum, static_cast<Enum>(value)>().isEmpty();
}

/**
 * @brief Create an @c int sequence of the given size starting from 0.
 *
 * @details @c int is used so that the sequence can hold any possible value for
 * the use case.
 *
 * @tparam size The size of the sequence.
 *
 * @return An @c int seqeunce starting from 0.
 */
template<std::size_t size>
constexpr auto createSequenceFromZero()
{
    return std::make_integer_sequence<int, size>{};
}

/**
 * @brief Offset the values in a sequence.
 *
 * @tparam Int The integer type.
 *
 * @tparam offset The offset value.
 *
 * @tparam values The values of the sequence.
 *
 * @param sequence The sequence.
 *
 * @return The sequence with the values offsetted.
 */
template<typename Int, Int offset, Int... values>
constexpr auto offsetBy(
    [[maybe_unused]] std::integer_sequence<Int, values...> sequence)
{
    return std::integer_sequence<Int, values + offset...>{};
}

/**
 * @brief Append a value to a sequence.
 *
 * @tparam Int The integer type.
 *
 * @tparam value The value to append.
 *
 * @tparam values The values of the sequence.
 *
 * @param sequence The sequence.
 *
 * @return The sequence with the value appended.
 */
template<typename Int, Int value, Int... values>
constexpr auto append(
    [[maybe_unused]] std::integer_sequence<Int, values...> sequence)
{
    return std::integer_sequence<Int, values..., value>{};
}

/**
 * @brief Cast the values of the sequence to a type.
 *
 * @tparam To The type to cast to.
 *
 * @tparam From The type of the values in the sequence.
 *
 * @tparam values The values of the sequence.
 *
 * @param sequence The sequence.
 *
 * @return The sequence with casted values.
 */
template<typename To, typename From, From... values>
constexpr auto castTo(
    [[maybe_unused]] std::integer_sequence<From, values...> sequence)
{
    return std::integer_sequence<To, values...>{};
}

/**
 * @brief Create an integer sequence from @c min to @c max.
 *
 * @tparam Int The integer type.
 *
 * @tparam min The minimum value of the sequence.
 *
 * @tparam max The maximum value of the sequence.
 *
 * @return An integer sequence from @c min to @c max.
 */
template<typename Int, Int min, Int max>
constexpr auto createRangedSequence()
{
    static_assert(min <= max);
    return append<Int, max>(
        offsetBy<Int, min>(castTo<Int>(createSequenceFromZero<max - min>())));
}

/**
 * @brief Count the number of valid enum values.
 *
 * @tparam Enum The enum type.
 *
 * @tparam size The number of enum values.
 *
 * @param enumValues The enum values to check for.
 *
 * @return The number of valid enum values.
 */
template<typename Enum, std::size_t size>
constexpr auto countValidEnumValues(
    const std::array<std::pair<Enum, bool>, size>& enumValues)
{
    std::size_t count = 0;
    for(const auto& [value, isValid] : enumValues) {
        if(isValid) {
            count++;
        }
    }
    return count;
}

/**
 * @brief Helper for @c getValues()
 *
 * @tparam Enum The enum type.
 *
 * @tparam values The sequence of values to check for.
 *
 * @param sequence The sequence.
 *
 * @return An array of valid enum values.
 */
template<typename Enum, std::underlying_type_t<Enum>... values>
constexpr auto getValuesHelper([[maybe_unused]] std::integer_sequence<
                               std::underlying_type_t<Enum>, values...>
                                   sequence)
{
    constexpr auto enumValues =
        std::array{std::make_pair(values, isValid<Enum, values>())...};
    std::array<Enum, countValidEnumValues(enumValues)> validEnumValues{};
    std::size_t index = 0;
    for(const auto& [value, isValid] : enumValues) {
        if(isValid) {
            validEnumValues.at(index) = static_cast<Enum>(value);
            index++;
        }
    }
    return validEnumValues;
}

/**
 * @brief Get an array of enum values of the enum type.
 *
 * @tparam Enum The enum type.
 *
 * @return An array of enum values of the enum type.
 */
template<typename Enum>
constexpr auto getValues()
{
    return getValuesHelper<Enum>(
        createRangedSequence<std::underlying_type_t<Enum>, Range<Enum>::min,
                             Range<Enum>::max>());
}

/**
 * @brief An array of enum values of the enum type.
 *
 * @tparam Enum The enum type.
 */
template<typename Enum>
inline constexpr auto values = detail::getValues<Enum>();

/**
 * @brief Helper for @c getNames().
 *
 * @tparam Enum The enum type.
 *
 * @tparam indexes The sequence of indexes.
 *
 * @param sequence The sequence.
 *
 * @return An array mapping of enum values to their name.
 */
template<typename Enum, std::size_t... indexes>
constexpr auto getNamesHelper(
    [[maybe_unused]] std::index_sequence<indexes...> sequence)
{
    return std::array{
        std::make_pair(values<Enum>.at(indexes),
                       name<Enum, values<Enum>.at(indexes)>.toStringView())...};
}

/**
 * @brief Get an array mapping of enum values to their name.
 *
 * @tparam Enum The enum type.
 *
 * @return An array mapping of enum values to their name.
 */
template<typename Enum>
constexpr auto getNames()
{
    return getNamesHelper<Enum>(
        std::make_index_sequence<values<Enum>.size()>{});
}

/**
 * @brief A mapping from enum values to their name.
 *
 * @tparam Enum The enum type.
 */
template<typename Enum>
inline constexpr auto names = detail::getNames<Enum>();
}

/**
 * @brief Get an array of enum values of the enum type.
 *
 * @tparam Enum The enum type.
 *
 * @return An array of enum values of the enum type.
 */
template<typename Enum>
constexpr const auto& getValues()
{
    return detail::values<Enum>;
}

/**
 * @brief Get an array mapping of enum values to their name.
 *
 * @tparam Enum The enum type.
 *
 * @return An array mapping of enum values to their name.
 */
template<typename Enum>
constexpr const auto& getNames()
{
    return detail::names<Enum>;
}

/**
 * @brief Get the name of the enum value.
 *
 * @tparam Enum The enum type.
 *
 * @param searchValue The enum value to get a name for.
 *
 * @return If the enum value is valid, the name of the enum value; otherwise, no
 * value.
 */
template<typename Enum>
constexpr std::optional<std::string_view> toName(Enum searchValue)
{
    // A `bool` and `std::string_view` is used here as opposed to an
    // `std::optional<std::string_view>` because C++17 prevents using
    // `std::optional::operator=(std::string_view)` in `constexpr` expressions
    bool isFound = false;
    std::string_view foundName;
    constexpr auto names = getNames<Enum>();
    for(const auto& [value, name] : names) {
        if(value == searchValue) {
            isFound = true;
            foundName = name;
            break;
        }
    }
    return isFound ? std::make_optional(foundName) : std::nullopt;
}

/**
 * @brief Get the enum value of the name.
 *
 * @tparam Enum The enum type.
 *
 * @param searchName The name of the enum value.
 *
 * @return If the name is valid, the enum value of the name; otherwise, no
 * value.
 */
template<typename Enum>
constexpr std::optional<Enum> fromName(std::string_view searchName)
{
    std::optional<Enum> foundValue;
    constexpr auto names = getNames<Enum>();
    for(const auto& [value, name] : names) {
        if(name == searchName) {
            foundValue = value;
        }
    }
    return foundValue;
}

/**
 * @brief Get the underlying integer value of the enum value.
 *
 * @tparam Enum The enum type.
 *
 * @param value The enum value.
 *
 * @return The underlying integer value of the enum value.
 */
template<typename Enum>
constexpr std::underlying_type_t<Enum> toUnderlying(Enum value)
{
    return static_cast<std::underlying_type_t<Enum>>(value);
}

/**
 * @brief Get the enum value of the underlying value.
 *
 * @tparam Enum The enum type.
 *
 * @param underlying The underlying value.
 *
 * @return If the underlying value is a valid enum value, the enum value;
 * otherwise, no value.
 */
template<typename Enum>
constexpr std::optional<Enum> fromUnderlying(
    std::underlying_type_t<Enum> underlying)
{
    std::optional<Enum> foundValue;
    constexpr auto values = getValues<Enum>();
    for(const auto& value : values) {
        if(value == static_cast<Enum>(underlying)) {
            foundValue = value;
        }
    }
    return foundValue;
}
}
