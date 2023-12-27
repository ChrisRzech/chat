#include "chat/messages/Message.hpp"

namespace chat::messages
{

Message::Message(Type type)
  : m_type{type}
{}

Message::Type Message::getType() const
{
    return m_type;
}

void Message::serialize(sf::Packet& packet) const
{
    packet << std::underlying_type_t<Type>(m_type);
}

}
