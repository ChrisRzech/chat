#pragma once

#include "chat/common/BufferView.hpp"
#include "chat/common/FixedBuffer.hpp"

#include <climits>
#include <cstddef>
#include <ostream>
#include <type_traits>

namespace chat::common::utility
{
/**
 * @brief Converts the bytes of an integral value into a buffer that is in
 * network byte order.
 *
 * @details When sending fields that are multi-byte, such as @c std::uint32_t (4
 * bytes), to another machine, the bytes are commonly sent in what is called the
 * network byte order. In RFC 1700 (https://www.rfc-editor.org/rfc/rfc1700.txt),
 * network byte order is defined to be big-endian.
 *
 * Initially, the bytes of a multi-byte field is considered to be in host byte
 * order, which is the native byte order of the machine. If the field needs to
 * be sent to another machine, the field is converted into the network byte
 * order and is sent to the other machine. Once a machine receives these bytes,
 * it is then converted into the host byte order. Although, if the host byte
 * order is the same byte order as network byte order, no real conversion needs
 * to take place.
 *
 * @tparam T The type of the integral value.
 *
 * @param value The integral value to be converted to network byte order.
 *
 * @return A buffer representing the integral value in network byte order.
 */
template<typename T>
constexpr FixedBuffer<sizeof(T)> toNetworkByteOrder(T value)
{
    static_assert(std::is_integral_v<T>, "Type is not an integral type");
    static_assert(!std::is_same_v<T, bool>,
                  "'bool' is not supported since 'std::make_unsigned' does not "
                  "support 'bool'");
    FixedBuffer<sizeof(T)> bytes = {};
    const std::make_unsigned_t<T> unsignedValue = value;
    for(std::size_t i = 0; i < bytes.size(); i++) {
        const unsigned int shift = ((bytes.size() - 1 - i) * CHAR_BIT);
        bytes.at(i) = static_cast<std::byte>(unsignedValue >> shift);
    }
    return bytes;
}

/**
 * @brief Converts a buffer in network byte order to an integral value in host
 * byte order.
 *
 * @details The opposite operation of @c toNetworkByteOrder().
 *
 * When receiving multi-byte fields, such as @c std::uint32_t (4 bytes), from
 * another machine, the bytes are commonly seen as being in network byte order.
 * It is possible that network byte order is not the native byte order of the
 * machine, therefore, this function makes sure that it is converted into the
 * native byte order.
 *
 * @note This function always assumes that the buffer is in network byte order.
 * Therefore, performing this operation on a buffer that is already in host byte
 * order might have no useful meaning.
 *
 * @tparam T The type of the integral value.
 *
 * @param bytes The buffer containing a network byte order representation of an
 * integral value.
 *
 * @return An integral value created from converting the buffer to host byte
 * order from network byte order.
 */
template<typename T>
constexpr T toHostByteOrder(const FixedBuffer<sizeof(T)>& bytes)
{
    static_assert(std::is_integral_v<T>, "Type is not an integral type");
    static_assert(!std::is_same_v<T, bool>,
                  "'bool' is not supported since 'std::make_unsigned' does not "
                  "support 'bool'");
    std::make_unsigned_t<T> unsignedValue = 0;
    for(std::size_t i = 0; i < bytes.size(); i++) {
        const unsigned int shift = (bytes.size() - 1 - i) * CHAR_BIT;
        unsignedValue |= static_cast<std::make_unsigned_t<T>>(
            static_cast<std::make_unsigned_t<T>>(bytes.at(i)) << shift);
    }
    return static_cast<T>(unsignedValue);
}

/**
 * @brief Outputs a string of bytes in a well-formatted hexadecimal
 * representation to an output stream.
 *
 * @details The format of the output is similar to the `xxd` or `hexdump`
 * commands.
 *
 * @param out The stream to output to.
 *
 * @param bytes The bytes to be printed.
 */
void hexdump(std::ostream& out, const BufferView& bytes);

/**
 * @brief Convert the value of an enum to its underlying type.
 *
 * @details This is equivalent to @c std::to_underlying() from the C++23
 * standard.
 *
 * @tparam Enum The enum type.
 *
 * @param value The enum value to convert.
 *
 * @return The converted value of the enum to its underlying type.
 */
template<typename Enum>
std::underlying_type_t<Enum> toUnderlying(Enum value)
{
    return static_cast<std::underlying_type_t<Enum>>(value);
}

/**
 * @brief Cast an integer into its signed version.
 *
 * @tparam T The integer type.
 *
 * @param value The integer value.
 *
 * @return The signed value of the integer value.
 */
template<typename T>
constexpr auto makeSigned(T value)
{
    return static_cast<std::make_signed_t<T>>(value);
}
}
