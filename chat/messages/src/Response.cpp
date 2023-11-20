#include "chat/messages/Response.hpp"

#include "chat/messages/TestResponse.hpp"

namespace chat::messages
{

Response::Response(Request::Type type)
  : Message{Type::Response},
    m_type{type}
{}

Request::Type Response::getType() const
{
    return m_type;
}

sf::Packet Response::toPacket() const
{
    auto packet = Message::toPacket();
    packet << static_cast<std::underlying_type_t<Type>>(m_type);
    return packet;
}

std::unique_ptr<Response> Response::createFromPacket(sf::Packet& packet)
{
    std::underlying_type_t<Request::Type> typeValue = 0;
    if(!(packet >> typeValue))
    {
        return nullptr;
    }

    std::unique_ptr<Response> response;
    switch(static_cast<Request::Type>(typeValue))
    {
    case Request::Type::Test:
        response = std::make_unique<TestResponse>();
        break;
    }

    return response;
}

}
