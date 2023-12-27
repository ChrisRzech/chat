#include "chat/messages/Request.hpp"

namespace chat::messages
{

Request::Request(Type type)
  : Message{Message::Type::Request},
    m_type{type}
{}

Request::Type Request::getType() const
{
    return m_type;
}

void Request::serialize(sf::Packet& packet) const
{
    Message::serialize(packet);
    packet << static_cast<std::underlying_type_t<Type>>(m_type);
}

}
