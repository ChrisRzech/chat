#include "chat/common/InputByteStream.hpp"

#include "chat/common/utility.hpp"

#include <type_traits>

namespace chat::common
{
namespace
{
template<typename T>
InputByteStream& readIntegral(InputByteStream& in, T& value)
{
    static_assert(std::is_integral_v<T>);
    FixedBuffer<sizeof(T)> buffer;
    if(in >> buffer) {
        value = utility::toHostByteOrder<T>(buffer);
    }
    return in;
}
}

InputByteStream::InputByteStream(BufferView buffer)
  : m_buffer{buffer},
    m_readIndex{0},
    m_failed{false}
{}

std::optional<BufferView> InputByteStream::read(std::size_t size)
{
    std::optional<BufferView> bytes;
    if(isEnoughBytes(size)) {
        bytes = std::make_optional(m_buffer.subspan(m_readIndex, size));
        m_readIndex += size;
    }
    return bytes;
}

bool InputByteStream::isGood() const
{
    return !m_failed;
}

bool InputByteStream::isEmpty() const
{
    return m_readIndex == m_buffer.size();
}

std::size_t InputByteStream::getReadableCount() const
{
    return m_buffer.size() - m_readIndex;
}

InputByteStream::operator bool() const
{
    return isGood();
}

bool InputByteStream::isEnoughBytes(std::size_t size)
{
    m_failed = size > m_buffer.size() - m_readIndex;
    return !m_failed;
}

InputByteStream& operator>>(InputByteStream& in, std::int8_t& value)
{
    return readIntegral(in, value);
}

InputByteStream& operator>>(InputByteStream& in, std::uint8_t& value)
{
    return readIntegral(in, value);
}

InputByteStream& operator>>(InputByteStream& in, std::int16_t& value)
{
    return readIntegral(in, value);
}

InputByteStream& operator>>(InputByteStream& in, std::uint16_t& value)
{
    return readIntegral(in, value);
}

InputByteStream& operator>>(InputByteStream& in, std::int32_t& value)
{
    return readIntegral(in, value);
}

InputByteStream& operator>>(InputByteStream& in, std::uint32_t& value)
{
    return readIntegral(in, value);
}

InputByteStream& operator>>(InputByteStream& in, std::int64_t& value)
{
    return readIntegral(in, value);
}

InputByteStream& operator>>(InputByteStream& in, std::uint64_t& value)
{
    return readIntegral(in, value);
}

InputByteStream& operator>>(InputByteStream& in, BufferView& buffer)
{
    std::uint32_t size = 0;
    if(in >> size) {
        auto bytes = in.read(size);
        if(bytes.has_value()) {
            buffer = bytes.value();
        }
    }
    return in;
}

InputByteStream& operator>>(InputByteStream& in, Buffer& buffer)
{
    std::uint32_t size = 0;
    if(in >> size) {
        auto bytes = in.read(static_cast<std::size_t>(size));
        if(bytes.has_value()) {
            buffer = Buffer{bytes.value().begin(), bytes.value().end()};
        }
    }
    return in;
}
}
