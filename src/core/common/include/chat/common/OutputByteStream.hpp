#pragma once

#include "chat/common/Buffer.hpp"
#include "chat/common/BufferView.hpp"
#include "chat/common/FixedBuffer.hpp"

#include <cstddef>
#include <cstdint>

namespace chat::common
{

/**
 * @brief An output stream of bytes.
 *
 * @details The stream is used to build a buffer which usually contains objects
 * that have been serialized into bytes.
 */
class OutputByteStream
{
public:
    /**
     * @brief Construct an output byte stream.
     */
    OutputByteStream() = default;

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    OutputByteStream(const OutputByteStream& other) = delete;
    OutputByteStream& operator=(const OutputByteStream& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    OutputByteStream(OutputByteStream&& other) = delete;
    OutputByteStream& operator=(OutputByteStream&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the output byte stream.
     */
    ~OutputByteStream() = default;

    /**
     * @brief Write bytes into the stream.
     *
     * @param bytes The bytes to use.
     */
    void write(const BufferView& bytes);

    /**
     * @brief Get the data the stream is building.
     *
     * @return The data the stream is building.
     */
    [[nodiscard]] const Buffer& getData() const;

private:
    Buffer m_buffer;
};

/**
 * @brief Insert a buffer into an output byte stream.
 *
 * @tparam N The size of the buffer.
 *
 * @param out The output byte stream.
 *
 * @param buffer The buffer to use.
 *
 * @return The output byte stream.
 */
template<std::size_t N>
inline OutputByteStream& operator<<(OutputByteStream& out,
                                    const FixedBuffer<N>& buffer)
{
    out.write(BufferView{buffer.data(), buffer.size()});
    return out;
}

/**
 * @brief Insert an integral value nto an output byte stream.
 *
 * @details The integral value is converted into network byte order (big-endian)
 * which is then inserted into the stream.
 *
 * @param out The output byte stream.
 *
 * @param value The integral value to use.
 *
 * @return The output byte stream.
 * @{
 */
OutputByteStream& operator<<(OutputByteStream& out, std::int8_t value);
OutputByteStream& operator<<(OutputByteStream& out, std::uint8_t value);
OutputByteStream& operator<<(OutputByteStream& out, std::int16_t value);
OutputByteStream& operator<<(OutputByteStream& out, std::uint16_t value);
OutputByteStream& operator<<(OutputByteStream& out, std::int32_t value);
OutputByteStream& operator<<(OutputByteStream& out, std::uint32_t value);
OutputByteStream& operator<<(OutputByteStream& out, std::int64_t value);
OutputByteStream& operator<<(OutputByteStream& out, std::uint64_t value);
/** @} */

/**
 * @brief Insert a buffer into an output byte stream.
 *
 * @details The size of the buffer is inserted into the stream first as a
 * @c std::uint32_t, and then the data of the buffer is inserted after.
 *
 * @param out The output byte stream.
 *
 * @param buffer The buffer to use.
 *
 * @return The output byte stream.
 */
OutputByteStream& operator<<(OutputByteStream& out, const BufferView& buffer);

/**
 * @brief Insert a buffer into an output byte stream.
 *
 * @details The size of the buffer is inserted into the stream first as a
 * @c std::uint32_t, and then the data of the buffer is inserted after.
 *
 * @details The size of the
 *
 * @param out The output byte stream.
 *
 * @param buffer The buffer to use.
 *
 * @return The output byte stream.
 */
OutputByteStream& operator<<(OutputByteStream& out, const Buffer& buffer);

}
