#include "chat/messages/Request.hpp"

#include "chat/common/OutputByteStream.hpp"

#include <type_traits>

namespace chat::messages
{
Request::Type Request::getType() const
{
    return m_type;
}

void Request::serialize(common::OutputByteStream& stream) const
{
    stream << static_cast<std::underlying_type_t<Type>>(m_type);
}

Request::Request(Type type)
  : m_type{type}
{}
}
