#include "chat/messages/Request.hpp"

namespace chat::messages
{

Request::Request(Type type)
  : Message{Message::Type::Request},
    m_type{type}
{}

Request::Type Request::getRequestType() const
{
    return m_type;
}

void Request::serialize(common::OutputByteStream& stream) const
{
    Message::serialize(stream);
    stream << static_cast<std::underlying_type_t<Type>>(m_type);
}

}
