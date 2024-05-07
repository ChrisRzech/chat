#pragma once

#include <cstddef>

namespace chat::common
{

/**
 * @brief An immutable reference to a contiguous sequence of bytes.
 *
 * @details The byte span does not manage the lifetime of the bytes it refers
 * to.
 */
class ByteSpan
{
public:
    /**
     * @brief Construct a byte span that refers to nothing.
     */
    ByteSpan();

    /**
     * @brief Construct a byte span.
     *
     * @param data The data to refer to.
     *
     * @param size The size of the data.
     */
    ByteSpan(const std::byte* data, std::size_t size);

    /**
     * @brief Copy operations are enabled.
     * @{
     */
    ByteSpan(const ByteSpan& other) = default;
    ByteSpan& operator=(const ByteSpan& other) = default;
    /** @} */

    /**
     * @brief Move operations are enabled.
     * @{
     */
    ByteSpan(ByteSpan&& other) = default;
    ByteSpan& operator=(ByteSpan&& other) = default;
    /** @} */

    /**
     * @brief Destroy the byte span.
     */
    ~ByteSpan() = default;

    /**
     * @brief Get a reference to the data.
     *
     * @return A reference to the data.
     */
    const std::byte* getData() const;

    /**
     * @brief Get the size of the data.
     *
     * @return The size of the data.
     */
    std::size_t getSize() const;

    /**
     * @brief Get a reference to a byte.
     *
     * @param index The index of the byte.
     *
     * @return A reference to the indexed byte.
     */
    const std::byte& operator[](std::size_t index) const;

    /**
     * @brief Create a new byte span from this byte span.
     *
     * @param offset The starting index of this byte span for the new byte span
     * to reference to.
     *
     * @param size The size of the new byte span.
     *
     * @return A byte span that refers to a subset of the bytes from this byte
     * span.
     */
    ByteSpan subspan(std::size_t offset, std::size_t size) const;

private:
    const std::byte* m_data;
    std::size_t m_size;
};

}
