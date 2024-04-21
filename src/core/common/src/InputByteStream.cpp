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
    ByteArray<sizeof(T)> buffer;
    if(in >> buffer)
    {
        value = utility::toHostByteOrder<T>(buffer);
    }
    return in;
}

}

InputByteStream::InputByteStream(ByteSpan buffer)
  : m_buffer{std::move(buffer)},
    m_readIndex{0},
    m_failed{false}
{}

std::optional<ByteSpan> InputByteStream::read(std::size_t size)
{
    std::optional<ByteSpan> bytes;
    if(isEnoughBytes(size))
    {
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
    return m_readIndex == m_buffer.getSize();
}

InputByteStream::operator bool() const
{
    return isGood();
}

bool InputByteStream::isEnoughBytes(std::size_t size)
{
    m_failed = size > m_buffer.getSize() - m_readIndex;
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

InputByteStream& operator>>(InputByteStream& in, ByteSpan& span)
{
    std::uint32_t size = 0;
    if(in >> size)
    {
        auto bytes = in.read(size);
        if(bytes.has_value())
        {
            span = bytes.value();
        }
    }
    return in;
}

InputByteStream& operator>>(InputByteStream& in, ByteString& buffer)
{
    std::uint32_t size = 0;
    if(in >> size)
    {
        auto bytes = in.read(static_cast<std::size_t>(size));
        if(bytes.has_value())
        {
            auto dataBegin = bytes.value().getData();
            auto dataEnd = dataBegin + bytes.value().getSize();
            buffer = ByteString{dataBegin, dataEnd};
        }
    }
    return in;
}

}
