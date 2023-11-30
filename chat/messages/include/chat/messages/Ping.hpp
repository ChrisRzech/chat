#pragma once

#include "chat/messages/Request.hpp"

#include <string>

namespace chat::messages
{

//TODO This is only a test, remove when done
class Ping : public Request
{
public:
    Ping()
      : Request{Type::Ping},
        m_message{}
    {}

    explicit Ping(std::string message)
      : Request{Type::Ping},
        m_message{std::move(message)}
    {}

    [[nodiscard]] std::string& getMessage()
    {
        return m_message;
    }

    void toPacket(sf::Packet& packet) const override
    {
        Request::toPacket(packet);
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
