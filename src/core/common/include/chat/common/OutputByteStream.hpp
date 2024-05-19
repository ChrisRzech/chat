#pragma once

#include "chat/common/ByteArray.hpp"
#include "chat/common/ByteSpan.hpp"
#include "chat/common/ByteString.hpp"

#include <cstddef>
#include <cstdint>

namespace chat::common
{

/**
 * @brief An output stream of bytes.
 *
 * @details The stream is used to build a byte string which usually contains
 * objects that have been serialized into bytes.
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
     * @param buffer The buffer of bytes to use.
     *
     * @param size The size of the buffer.
     */
    void write(const std::byte* buffer, std::size_t size);

    /**
     * @brief Get the data the stream is building.
     *
     * @return The data the stream is building.
     */
    [[nodiscard]] const ByteString& getData() const;

private:
    ByteString m_buffer;
};

/**
 * @brief Insert a byte array into an output byte stream.
 *
 * @tparam N The size of the byte array.
 *
 * @param out The output byte stream.
 *
 * @param buffer The byte array to use.
 *
 * @return The output byte stream.
 */
template<std::size_t N>
inline OutputByteStream& operator<<(OutputByteStream& out,
                                    const ByteArray<N>& buffer)
{
    out.write(buffer.data(), buffer.size());
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
 * @brief Insert a byte span into an output byte stream.
 *
 * @details The size of the byte span is inserted into the stream first as a
 * @c std::uint32_t, and then the data of the byte span is inserted after.
 *
 * @param out The output byte stream.
 *
 * @param span The byte span to use.
 *
 * @return The output byte stream.
 */
OutputByteStream& operator<<(OutputByteStream& out, const ByteSpan& span);

/**
 * @brief Insert a byte string into an output byte stream.
 *
 * @details The size of the byte string is inserted into the stream first as a
 * @c std::uint32_t, and then the data of the byte string is inserted after.
 *
 * @details The size of the
 *
 * @param out The output byte stream.
 *
 * @param span The byte string to use.
 *
 * @return The output byte stream.
 */
OutputByteStream& operator<<(OutputByteStream& out, const ByteString& buffer);

}
