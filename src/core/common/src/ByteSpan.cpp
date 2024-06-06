#include "chat/common/ByteSpan.hpp"

namespace chat::common
{

ByteSpan::ByteSpan()
  : m_data{nullptr},
    m_size{0}
{}

ByteSpan::ByteSpan(const std::byte* data, std::size_t size)
  : m_data{data},
    m_size{size}
{}

const std::byte* ByteSpan::getData() const
{
    return m_data;
}

std::size_t ByteSpan::getSize() const
{
    return m_size;
}

const std::byte& ByteSpan::operator[](std::size_t index) const
{
    // The purpose of this class is to prevent having to use pointers
    // arithmetic. However, to do so, pointer arithmetic must be used.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return m_data[index];
}

const std::byte* ByteSpan::begin() const
{
    return m_data;
}

const std::byte* ByteSpan::end() const
{
    // The purpose of this class is to prevent having to use pointers
    // arithmetic. However, to do so, pointer arithmetic must be used.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return m_data + m_size;
}

ByteSpan ByteSpan::subspan(std::size_t offset, std::size_t size) const
{
    // The purpose of this class is to prevent having to use pointers
    // arithmetic. However, to do so, pointer arithmetic must be used.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return ByteSpan{m_data + offset, size};
}

}
