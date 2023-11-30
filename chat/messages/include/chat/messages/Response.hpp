#pragma once

#include "chat/messages/Message.hpp"

#include <SFML/Network/Packet.hpp>

#include <cstdint>

namespace chat::messages
{

class Response : public Message
{
public:
    enum class Type : uint32_t
    {
        Pong
    };

    Response(const Response& other) = delete;

    Response& operator=(const Response& other) = delete;

    Response(Response&& other) = default;

    Response& operator=(Response&& other) = default;

    ~Response() override = default;

    [[nodiscard]] Type getType() const;

    void toPacket(sf::Packet& packet) const override;

protected:
    explicit Response(Type type);

private:
    Type m_type;
};

}
