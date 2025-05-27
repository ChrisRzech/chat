#pragma once

#include <cstddef>

namespace chat::common
{

/**
 * @brief An immutable non-owning view to a contiguous sequence of bytes.
 */
class BufferView
{
public:
    /**
     * @brief Construct a view that refers to nothing.
     */
    BufferView();

    /**
     * @brief Construct a view.
     *
     * @param data The data to refer to.
     *
     * @param size The size of the data.
     */
    BufferView(const std::byte* data, std::size_t size);

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
     * @brief Create a new view from this view.
     *
     * @param offset The starting index of this view for the new view to
     * reference to.
     *
     * @param size The size of the new view.
     *
     * @return A view that refers to a subset of the bytes from this view.
     */
    [[nodiscard]] BufferView subview(std::size_t offset,
                                     std::size_t size) const;

    /**
     * @brief Check if view and another view contains the same bytes.
     *
     * @param other Another view to compare this one to.
     *
     * @return True if both views contains the same bytes; otherwise, false.
     */
    [[nodiscard]] bool operator==(const BufferView& other) const;

    /**
     * @brief Check if this view and another view do not contain the same
     * bytes.
     *
     * @param other Another view to compare this one to.
     *
     * @return True if both views contains the same bytes; otherwise, false.
     */
    [[nodiscard]] bool operator!=(const BufferView& other) const;

private:
    const std::byte* m_data;
    std::size_t m_size;
};

}
