#pragma once

#include "chat/common/Buffer.hpp"
#include "chat/common/BufferView.hpp"
#include "chat/common/FixedBuffer.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>

namespace chat::common
{

/**
 * @brief An input stream of bytes.
 *
 * @details The stream does not own a buffer but instead holds a view to a
 * previously allocated buffer. It's important to ensure that the buffer remains
 * valid throughout the lifespan of this stream.
 *
 * Reading bytes provides subviews into the input buffer rather than providing
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
    explicit InputByteStream(BufferView buffer);

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
    [[nodiscard]] std::optional<BufferView> read(std::size_t size);

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

    BufferView m_buffer;
    std::size_t m_readIndex;
    bool m_failed;
};

/**
 * @brief Extract bytes from an input byte stream into a buffer.
 *
 * @tparam N The size of the buffer.
 *
 * @param in The input byte stream.
 *
 * @param buffer The buffer to fill.
 *
 * @return The input byte stream.
 */
template<std::size_t N>
inline InputByteStream& operator>>(InputByteStream& in, FixedBuffer<N>& buffer)
{
    auto bytes = in.read(buffer.size());
    if(bytes.has_value()) {
        for(std::size_t i = 0; i < buffer.size(); i++) {
            buffer.at(i) = bytes.value()[i];
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
 * @brief Extract bytes from an input byte stream into a buffer.
 *
 * @details This assumes that the stream contains a @c std::uint32_t, to specify
 * the size of the buffer, and then the bytes with the extracted size.
 *
 * @param in The input byte stream.
 *
 * @param buffer The buffer to which the extracted buffer will be assigned to.
 *
 * @return The input byte stream.
 */
InputByteStream& operator>>(InputByteStream& in, BufferView& buffer);

/**
 * @brief Extract bytes from an input byte stream into a buffer.
 *
 * @details This assumes that the stream contains a @c std::uint32_t, to specify
 * the size of the buffer, and then the bytes with the extracted size.
 *
 * @param in The input byte stream.
 *
 * @param buffer The buffer to which the extract buffer will be assigned to.
 *
 * @return The input byte stream.
 */
InputByteStream& operator>>(InputByteStream& in, Buffer& buffer);

}
