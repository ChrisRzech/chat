#pragma once

#include "chat/messages/Message.hpp"

#include <SFML/Network/Packet.hpp>

#include <cstdint>

namespace chat::messages
{

class Request : public Message
{
public:
    enum class Type : uint32_t
    {
        Ping
    };

    Request(const Request& other) = delete;

    Request& operator=(const Request& other) = delete;

    Request(Request&& other) = default;

    Request& operator=(Request&& other) = default;

    ~Request() override = default;

    [[nodiscard]] Type getType() const;

    void toPacket(sf::Packet& packet) const override;

protected:
    explicit Request(Type type);

private:
    Type m_type;
};

}
