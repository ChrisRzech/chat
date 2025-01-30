#pragma once

#include "chat/common/ByteArray.hpp"
#include "chat/common/ByteSpan.hpp"
#include "chat/common/ByteString.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>

namespace chat::common
{

/**
 * @brief An input stream of bytes.
 *
 * @details The stream does not own a byte string but instead holds a span to a
 * previously allocated byte string. It's important to ensure that the byte
 * string remains valid throughout the lifespan of this stream.
 *
 * Reading bytes provides subspans into the input span rather than providing
 * copies. This is to reduce the memory usage for use cases where copies do not
 * need to be made. The stream will keep track if the last read operation was
 * successful using @c isGood().
 */
class InputByteStream
{
public:
    /**
     * @brief Construct an input byte stream.
     *
     * @param buffer The bytes for the stream to use.
     */
    explicit InputByteStream(ByteSpan buffer);

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    InputByteStream(const InputByteStream& other) = delete;
    InputByteStream& operator=(const InputByteStream& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    InputByteStream(InputByteStream&& other) = delete;
    InputByteStream& operator=(InputByteStream&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the input byte stream.
     */
    ~InputByteStream() = default;

    /**
     * @brief Read bytes from the stream.
     *
     * @details This is successful if there is a minimum number of readable
     * bytes left to fullfil the requested size. Once bytes are read, they are
     * no longer readable again.
     *
     * @param size The amount of bytes to read.
     *
     * @return A value if there is a minimum number of readable bytes left to
     * fullfil the requested size; otherwise, no value.
     */
    [[nodiscard]] std::optional<ByteSpan> read(std::size_t size);

    /**
     * @brief Check if the last read was successful.
     *
     * @return True if the last read was successful; otherwise, false.
     */
    [[nodiscard]] bool isGood() const;

    /**
     * @brief Check if there are readable bytes left.
     *
     * @return True if there are readable bytes left; otherwise, false.
     */
    [[nodiscard]] bool isEmpty() const;

    /**
     * @brief Get the number of readable bytes from the stream.
     *
     * @return The number of readable bytes from the stream.
     */
    [[nodiscard]] std::size_t getReadableCount() const;

    /**
     * @brief Check if the last read was successful.
     *
     * @details Equivalent to @c InputByteStream::isGood().
     *
     * @return True if the last read was successful; otherwise, false.
     */
    explicit operator bool() const;

private:
    /**
     * @brief Check if there is a minimum number of readable bytes left to
     * fullfil the requested size.
     *
     * @param size The number of bytes to check for.
     *
     * @return True if there is a minimum number of readable bytes left to
     * fullfil the requested size; otherwise, false.
     */
    [[nodiscard]] bool isEnoughBytes(std::size_t size);

    ByteSpan m_buffer;
    std::size_t m_readIndex;
    bool m_failed;
};

/**
 * @brief Extract bytes from an input byte stream into a byte array.
 *
 * @tparam N The size of the byte array.
 *
 * @param in The input byte stream.
 *
 * @param buffer The byte array to fill.
 *
 * @return The input byte stream.
 */
template<std::size_t N>
inline InputByteStream& operator>>(InputByteStream& in, ByteArray<N>& buffer)
{
    auto bytes = in.read(buffer.size());
    if(bytes.has_value()) {
        for(std::size_t i = 0; i < buffer.size(); i++) {
            buffer[i] = bytes.value()[i];
        }
    }
    return in;
}

/**
 * @brief Extract bytes from an input byte stream into an integral.
 *
 * @details The bytes used from the stream are assumed to be in network byte
 * order (big-endian), and this function converts those bytes into the host byte
 * order (little-endian, big-endian, etc).
 *
 * @tparam T The type of the integral value.
 *
 * @param in The input byte stream.
 *
 * @param value The integral value to fill.
 *
 * @return The input byte stream.
 * @{
 */
InputByteStream& operator>>(InputByteStream& in, std::int8_t& value);
InputByteStream& operator>>(InputByteStream& in, std::uint8_t& value);
InputByteStream& operator>>(InputByteStream& in, std::int16_t& value);
InputByteStream& operator>>(InputByteStream& in, std::uint16_t& value);
InputByteStream& operator>>(InputByteStream& in, std::int32_t& value);
InputByteStream& operator>>(InputByteStream& in, std::uint32_t& value);
InputByteStream& operator>>(InputByteStream& in, std::int64_t& value);
InputByteStream& operator>>(InputByteStream& in, std::uint64_t& value);
/** @} */

/**
 * @brief Extract bytes from an input byte stream into a byte span.
 *
 * @details This assumes that the stream contains a @c std::uint32_t, to specify
 * the size of the span, and then the bytes with the extracted size.
 *
 * @param in The input byte stream.
 *
 * @param span The byte span to which the extracted span will be assigned to.
 *
 * @return The input byte stream.
 */
InputByteStream& operator>>(InputByteStream& in, ByteSpan& span);

/**
 * @brief Extract bytes from an input byte stream into a byte string.
 *
 * @details This assumes that the stream contains a @c std::uint32_t, to specify
 * the size of the string, and then the bytes with the extracted size.
 *
 * @param in The input byte stream.
 *
 * @param buffer The byte string to which the extract string will be assigned
 * to.
 *
 * @return The input byte stream.
 */
InputByteStream& operator>>(InputByteStream& in, ByteString& buffer);

}
