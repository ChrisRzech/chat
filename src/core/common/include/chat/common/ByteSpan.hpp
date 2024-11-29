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
     * @brief Get a pointer to the start of the data.
     *
     * @return A pointer to the start of the data.
     */
    [[nodiscard]] const std::byte* getData() const;

    /**
     * @brief Get the size of the data.
     *
     * @return The size of the data.
     */
    [[nodiscard]] std::size_t getSize() const;

    /**
     * @brief Get a reference to a byte.
     *
     * @param index The index of the byte.
     *
     * @return A reference to the indexed byte.
     */
    [[nodiscard]] const std::byte& operator[](std::size_t index) const;

    /**
     * @brief Get an iterator to the first element.
     *
     * @return An iterator to the first element.
     */
    [[nodiscard]] const std::byte* begin() const;

    /**
     * @brief Get an iterator to the element following the last element.
     *
     * @details The iterator returned is a placeholder and should not be
     * accessed.
     *
     * @return An iterator to the element following the last element.
     */
    [[nodiscard]] const std::byte* end() const;

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
    [[nodiscard]] ByteSpan subspan(std::size_t offset, std::size_t size) const;

    /**
     * @brief Check if this span and another span contains the same bytes.
     *
     * @param other Another span to compare this one to.
     *
     * @return True if both spans contains the same bytes; otherwise, false.
     */
    [[nodiscard]] bool operator==(const ByteSpan& other) const;

    /**
     * @brief Check if this span and another span do not contain the same
     * bytes.
     *
     * @param other Another span to compare this one to.
     *
     * @return True if both spans contains the same bytes; otherwise, false.
     */
    [[nodiscard]] bool operator!=(const ByteSpan& other) const;

private:
    const std::byte* m_data;
    std::size_t m_size;
};

}
