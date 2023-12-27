#pragma once

#include "chat/messages/Message.hpp"

#include <SFML/Network/Packet.hpp>

#include <cstdint>

namespace chat::messages
{

/**
 * @brief A response message.
 *
 * @details A response is sent from the server to the client only in reply to a @c Request.
 */
class Response : public Message
{
public:
    /**
     * @brief The type of a response.
     */
    enum class Type : uint32_t
    {
        Pong
    };

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Response(const Response& other) = delete;
    Response& operator=(const Response& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Response(Response&& other) = delete;
    Response& operator=(Response&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the response.
     */
    ~Response() override = default;

    /**
     * @brief Get the type of the response.
     *
     * @return The type of the response.
     */
    [[nodiscard]] Type getType() const;

    /**
     * @brief Serialize the message into a packet.
     *
     * @param packet The packet to serialize the message into.
     */
    void serialize(sf::Packet& packet) const override;

protected:
    /**
     * @brief Construct a response.
     *
     * @param type The type of the response.
     */
    explicit Response(Type type);

private:
    Type m_type;
};

}
