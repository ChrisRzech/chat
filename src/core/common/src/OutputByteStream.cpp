#include "chat/common/OutputByteStream.hpp"

#include "chat/common/utility.hpp"

#include <type_traits>

namespace chat::common
{

namespace
{

template<typename T>
OutputByteStream& writeIntegral(OutputByteStream& out, const T& value)
{
    static_assert(std::is_integral_v<T>);
    return out << utility::toNetworkByteOrder(value);
}

}

void OutputByteStream::write(const ByteSpan& bytes)
{
    m_buffer.insert(m_buffer.end(), bytes.begin(), bytes.end());
}

const Buffer& OutputByteStream::getData() const
{
    return m_buffer;
}

OutputByteStream& operator<<(OutputByteStream& out, std::int8_t value)
{
    return writeIntegral(out, value);
}

OutputByteStream& operator<<(OutputByteStream& out, std::uint8_t value)
{
    return writeIntegral(out, value);
}

OutputByteStream& operator<<(OutputByteStream& out, std::int16_t value)
{
    return writeIntegral(out, value);
}

OutputByteStream& operator<<(OutputByteStream& out, std::uint16_t value)
{
    return writeIntegral(out, value);
}

OutputByteStream& operator<<(OutputByteStream& out, std::int32_t value)
{
    return writeIntegral(out, value);
}

OutputByteStream& operator<<(OutputByteStream& out, std::uint32_t value)
{
    return writeIntegral(out, value);
}

OutputByteStream& operator<<(OutputByteStream& out, std::int64_t value)
{
    return writeIntegral(out, value);
}

OutputByteStream& operator<<(OutputByteStream& out, std::uint64_t value)
{
    return writeIntegral(out, value);
}

OutputByteStream& operator<<(OutputByteStream& out, const ByteSpan& span)
{
    out << static_cast<std::uint32_t>(span.getSize());
    out.write(span);
    return out;
}

OutputByteStream& operator<<(OutputByteStream& out, const Buffer& buffer)
{
    return out << ByteSpan{buffer.data(), buffer.size()};
}

}
