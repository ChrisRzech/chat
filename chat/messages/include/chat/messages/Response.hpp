#pragma once

#include "chat/messages/Message.hpp"
#include "chat/messages/Request.hpp"

#include <memory>

namespace chat::messages
{

class Response : public Message
{
protected:
    Response(Request::Type type);

public:
    Response(const Response& other) = delete;

    Response& operator=(const Response& other) = delete;

    Response(Response&& other) = default;

    Response& operator=(Response&& other) = default;

    ~Response() override = default;

    [[nodiscard]] Request::Type getType() const;

    [[nodiscard]] sf::Packet toPacket() const override;

    [[nodiscard]] static std::unique_ptr<Response> createFromPacket(sf::Packet& packet);

private:
    Request::Type m_type;
};

}
