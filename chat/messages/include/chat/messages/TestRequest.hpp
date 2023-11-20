#pragma once

#include "chat/messages/Request.hpp"

#include <string>

namespace chat::messages
{

//TODO This is only a test, remove when done
class TestRequest : public Request
{
public:
    TestRequest()
      : Request{Type::Test},
        m_message{}
    {}

    explicit TestRequest(std::string message)
      : Request{Type::Test},
        m_message{std::move(message)}
    {}

    std::string& getMessage()
    {
        return m_message;
    }

    sf::Packet toPacket() const override
    {
        auto packet = Request::toPacket();
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
