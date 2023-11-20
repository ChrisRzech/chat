#pragma once

#include "chat/messages/Message.hpp"

#include <SFML/Network/Packet.hpp>

#include <cstdint>
#include <memory>

namespace chat::messages
{

class Request : public Message
{
public:
    enum class Type : uint8_t
    {
        Test
    };

protected:
    explicit Request(Type type);

public:
    Request(const Request& other) = delete;

    Request& operator=(const Request& other) = delete;

    Request(Request&& other) = default;

    Request& operator=(Request&& other) = default;

    ~Request() override = default;

    [[nodiscard]] Type getType() const;

    [[nodiscard]] sf::Packet toPacket() const override;

    [[nodiscard]] static std::unique_ptr<Request> createFromPacket(sf::Packet& packet);

private:
    Type m_type;
};

}
