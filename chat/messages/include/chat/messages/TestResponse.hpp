#pragma once

#include "chat/messages/Response.hpp"

#include <string>

namespace chat::messages
{

//TODO This is only a test, remove when done
class TestResponse : public Response
{
public:
    TestResponse()
      : Response{Request::Type::Test},
        m_message{}
    {}

    explicit TestResponse(std::string message)
      : Response{Request::Type::Test},
        m_message{std::move(message)}
    {}

    const std::string& getMessage() const
    {
        return m_message;
    }

    sf::Packet toPacket() const override
    {
        auto packet = Response::toPacket();
        packet << m_message;
        return packet;
    }

    bool fromPacket(sf::Packet& packet) override
    {
        return packet >> m_message;
    }

private:
    std::string m_message;
};

}
