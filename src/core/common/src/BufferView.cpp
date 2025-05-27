#include "chat/common/BufferView.hpp"

#include <algorithm>

namespace chat::common
{

BufferView::BufferView()
  : m_data{nullptr},
    m_size{0}
{}

BufferView::BufferView(const std::byte* data, std::size_t size)
  : m_data{data},
    m_size{size}
{}

const std::byte* BufferView::getData() const
{
    return m_data;
}

std::size_t BufferView::getSize() const
{
    return m_size;
}

const std::byte& BufferView::operator[](std::size_t index) const
{
    // The purpose of this class is to prevent having to use pointers
    // arithmetic. However, to do so, pointer arithmetic must be used.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return m_data[index];
}

const std::byte* BufferView::begin() const
{
    return m_data;
}

const std::byte* BufferView::end() const
{
    // The purpose of this class is to prevent having to use pointers
    // arithmetic. However, to do so, pointer arithmetic must be used.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return m_data + m_size;
}

BufferView BufferView::subview(std::size_t offset, std::size_t size) const
{
    // The purpose of this class is to prevent having to use pointers
    // arithmetic. However, to do so, pointer arithmetic must be used.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return BufferView{m_data + offset, size};
}

bool BufferView::operator==(const BufferView& other) const
{
    return m_size == other.m_size ? std::equal(begin(), end(), other.begin())
                                  : false;
}

bool BufferView::operator!=(const BufferView& other) const
{
    return !operator==(other);
}

}
