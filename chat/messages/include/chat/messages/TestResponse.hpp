#pragma once

#include "chat/messages/Response.hpp"

namespace chat::messages
{

class TestResponse : public Response
{
public:
    TestResponse() = default;

    explicit TestResponse(std::string message)
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
