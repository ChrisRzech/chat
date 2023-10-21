#pragma once

#include "chat/messages/Request.hpp"

namespace chat::messages
{

class TestRequest : public Request
{
public:
    TestRequest() = default;

    explicit TestRequest(std::string message)
      : m_message{std::move(message)}
    {}

    const std::string& getMessage() const
    {
        return m_message;
    }

    sf::Packet toPacket() const override
    {
        sf::Packet packet;
        packet << m_message;
        return packet;
    }

    void fromPacket(sf::Packet& packet) override
    {
        packet >> m_message;
    }

private:
    std::string m_message;
};

}
