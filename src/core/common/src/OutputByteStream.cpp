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

void OutputByteStream::write(const BufferView& bytes)
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

OutputByteStream& operator<<(OutputByteStream& out, const BufferView& buffer)
{
    out << static_cast<std::uint32_t>(buffer.getSize());
    out.write(buffer);
    return out;
}

OutputByteStream& operator<<(OutputByteStream& out, const Buffer& buffer)
{
    return out << BufferView{buffer.data(), buffer.size()};
}

}
