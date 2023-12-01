#pragma once

#include "chat/messages/Response.hpp"

#include <string>

namespace chat::messages
{

class Pong : public Response
{
public:
    Pong();

    Pong(const Pong& other) = delete;

    Pong& operator=(const Pong& other) = delete;

    Pong(Pong&& other) = default;

    Pong& operator=(Pong&& other) = default;

    ~Pong() override = default;

    void toPacket(sf::Packet& packet) const override;

    [[nodiscard]] bool fromPacket(sf::Packet& packet) override;
};

}
