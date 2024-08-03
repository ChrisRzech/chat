#include "chat/messages/Response.hpp"

namespace chat::messages
{

Response::Response(Type type)
  : Message{Message::Type::Response},
    m_type{type}
{}

Response::Type Response::getResponseType() const
{
    return m_type;
}

void Response::serialize(common::OutputByteStream& stream) const
{
    Message::serialize(stream);
    stream << static_cast<std::underlying_type_t<Type>>(m_type);
}

}
