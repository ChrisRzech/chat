#include "chat/messages/Request.hpp"

#include "chat/common/Logging.hpp"

#include "chat/messages/TestRequest.hpp"

#include <stdexcept>

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

sf::Packet Request::toPacket() const
{
    auto packet = Message::toPacket();
    packet << static_cast<std::underlying_type_t<Type>>(m_type);
    return packet;
}

std::unique_ptr<Request> Request::createFromPacket(sf::Packet& packet)
{
    std::underlying_type_t<Type> typeValue = 0;
    if(!(packet >> typeValue))
    {
        return nullptr;
    }

    std::unique_ptr<Request> request;
    switch(static_cast<Type>(typeValue))
    {
    case Type::Test:
        request = std::make_unique<TestRequest>();
        break;
    }

    return request;
}

}
