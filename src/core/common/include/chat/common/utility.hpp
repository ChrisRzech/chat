#pragma once

#include "chat/common/ByteArray.hpp"

#include <climits>
#include <cstddef>
#include <ostream>
#include <type_traits>

namespace chat::common::utility
{

/**
 * @brief Converts the bytes of an integral value into a byte array that is in network byte order.
 *
 * @details When sending fields that are multi-byte, such as @c std::uint32_t (4 bytes), to another machine, the bytes are commonly sent in
 * what is called the network byte order. In RFC 1700 (https://www.rfc-editor.org/rfc/rfc1700.txt), network byte order is defined to be
 * big-endian.
 *
 * Initially, the bytes of a multi-byte field is considered to be in host byte order, which is the native byte order of the machine. If the
 * field needs to be sent to another machine, the field is converted into the network byte order and is sent to the other machine. Once a
 * machine receives these bytes, it is then converted into the host byte order. Although, if the host byte order is the same byte order as
 * network byte order, no real conversion needs to take place.
 *
 * @tparam T The type of the integral value.
 *
 * @param value The integral value to be converted to network byte order.
 *
 * @return A byte array representing the integral value in network byte order.
 */
template<typename T>
constexpr ByteArray<sizeof(T)> toNetworkByteOrder(T value)
{
    static_assert(std::is_integral_v<T>, "Type is not an integral type");
    static_assert(!std::is_same_v<T, bool>, "'bool' is not supported since 'std::make_unsigned' does not support 'bool'");
    ByteArray<sizeof(T)> bytes = {};
    for(std::size_t i = 0; i < bytes.size(); i++)
    {
        constexpr auto BYTE_MASK = (1 << CHAR_BIT) - 1;
        bytes[i] = static_cast<std::byte>((value >> ((bytes.size() - 1 - i) * CHAR_BIT)) & BYTE_MASK);
    }
    return bytes;
}

/**
 * @brief Converts a byte array in network byte order to an integral value in host byte order.
 *
 * @details The opposite operation of @c toNetworkByteOrder().
 *
 * When receiving multi-byte fields, such as @c std::uint32_t (4 bytes), from another machine, the bytes are commonly seen as being in
 * network byte order. It is possible that network byte order is not the native byte order of the machine, therefore, this function makes
 * sure that it is converted into the native byte order.
 *
 * @note This function always assumes that the byte array is in network byte order. Therefore, performing this operation on a byte array
 * that is already in host byte order might have no useful meaning.
 *
 * @tparam T The type of the integral value.
 *
 * @param bytes The byte array containing a network byte order representation of an integral value.
 *
 * @return An integral value created from converting the byte array to host byte order from network byte order.
 */
template<typename T>
constexpr T toHostByteOrder(const ByteArray<sizeof(T)>& bytes)
{
    static_assert(std::is_integral_v<T>, "Type is not an integral type");
    static_assert(!std::is_same_v<T, bool>, "'bool' is not supported since 'std::make_unsigned' does not support 'bool'");
    T value = 0;
    for(std::size_t i = 0; i < bytes.size(); i++)
    {
        value |= static_cast<T>(static_cast<std::make_unsigned_t<T>>(bytes[i]) << ((bytes.size() - 1 - i) * CHAR_BIT));
    }
    return value;
}

/**
 * @brief Outputs a string of bytes in a well-formatted hexadecimal representation to an output stream.
 *
 * @details The format of the output is similar to the `xxd` or `hexdump` commands.
 *
 * @param out The stream to output to.
 *
 * @param bytes A pointer to the bytes to be printed.
 *
 * @param size The number of bytes to print.
 */
void hexdump(std::ostream& out, const std::byte* bytes, std::size_t size);

}
