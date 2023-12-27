#pragma once

#include "chat/messages/Response.hpp"

namespace chat::messages
{

/**
 * @brief A pong response.
 */
class Pong : public Response
{
public:
    /**
     * @brief Construct a pong response.
     */
    Pong();

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Pong(const Pong& other) = delete;
    Pong& operator=(const Pong& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Pong(Pong&& other) = delete;
    Pong& operator=(Pong&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the pong response.
     */
    ~Pong() override = default;

    /**
     * @brief Serialize the message into a packet.
     *
     * @param packet The packet to serialize the message into.
     */
    void serialize(sf::Packet& packet) const override;

    /**
     * @brief Deserialize the message from a packet.
     *
     * @param packet The packet to deserialize the message from.
     *
     * @return True if the message successfully deserialized; otherwise, false.
     */
    [[nodiscard]] bool deserialize(sf::Packet& packet) override;
};

}
