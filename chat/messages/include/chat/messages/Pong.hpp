#pragma once

#include "chat/messages/Response.hpp"

#include <string>

namespace chat::messages
{

//TODO This is only a test, remove when done
class Pong : public Response
{
public:
    Pong()
      : Response{Type::Pong},
        m_message{}
    {}

    explicit Pong(std::string message)
      : Response{Type::Pong},
        m_message{std::move(message)}
    {}

    [[nodiscard]] std::string& getMessage()
    {
        return m_message;
    }

    void toPacket(sf::Packet& packet) const override
    {
        Response::toPacket(packet);
        packet << m_message;
    }

    [[nodiscard]] bool fromPacket(sf::Packet& packet) override
    {
        return packet >> m_message;
    }

private:
    std::string m_message;
};

}
