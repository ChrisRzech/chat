#include "chat/messages/Response.hpp"

namespace chat::messages
{

Response::Response(Type type)
  : Message{Message::Type::Response},
    m_type{type}
{}

Response::Type Response::getType() const
{
    return m_type;
}

void Response::serialize(sf::Packet& packet) const
{
    Message::serialize(packet);
    packet << static_cast<std::underlying_type_t<Type>>(m_type);
}

}
