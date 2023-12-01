#pragma once

#include "chat/messages/Request.hpp"

#include <string>

namespace chat::messages
{

class Ping : public Request
{
public:
    Ping();

    Ping(const Ping& other) = delete;

    Ping& operator=(const Ping& other) = delete;

    Ping(Ping&& other) = default;

    Ping& operator=(Ping&& other) = default;

    ~Ping() override = default;

    void toPacket(sf::Packet& packet) const override;

    [[nodiscard]] bool fromPacket(sf::Packet& packet) override;
};

}
