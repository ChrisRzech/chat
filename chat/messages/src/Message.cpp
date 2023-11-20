#include "chat/messages/Message.hpp"

#include "chat/messages/Close.hpp"
#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include <type_traits>

namespace chat::messages
{

Message::Message(Type type)
  : m_type{type}
{}

Message::Type Message::getType() const
{
    return m_type;
}

sf::Packet Message::toPacket() const
{
    sf::Packet packet;
    packet << static_cast<std::underlying_type_t<Type>>(m_type);
    return packet;
}

std::unique_ptr<Message> Message::createFromPacket(sf::Packet& packet)
{
    std::underlying_type_t<Type> typeValue = 0;
    if(!(packet >> typeValue))
    {
        return nullptr;
    }

    std::unique_ptr<Message> message;
    switch(static_cast<Type>(typeValue))
    {
    case Type::Close:
        message = std::make_unique<class Close>();
        break;
    case Type::Request:
        message = Request::createFromPacket(packet);
        break;
    case Type::Response:
        message = Response::createFromPacket(packet);
        break;
    }

    if(message != nullptr && !message->fromPacket(packet))
    {
        return nullptr;
    }

    return message;
}

}
