#include "chat/messages/Response.hpp"

namespace chat::messages
{
Response::Type Response::getType() const
{
    return m_type;
}

void Response::serialize(common::OutputByteStream& stream) const
{
    stream << static_cast<std::underlying_type_t<Type>>(m_type);
}

Response::Response(Type type)
  : m_type{type}
{}
}
