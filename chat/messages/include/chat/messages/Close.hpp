#pragma once

#include "chat/messages/Message.hpp"

#include <SFML/Network/Packet.hpp>

namespace chat::messages
{

class Close : public Message
{
public:
    Close();

    Close(const Close& other) = delete;

    Close& operator=(const Close& other) = delete;

    Close(Close&& other) = default;

    Close& operator=(Close&& other) = default;

    ~Close() override = default;

    void toPacket(sf::Packet& packet) const override;

    [[nodiscard]] bool fromPacket(sf::Packet& packet) override;
};

}
