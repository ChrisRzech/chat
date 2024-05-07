#pragma once

#include "chat/messages/Message.hpp"

#include <SFML/Network/Packet.hpp>

namespace chat::messages
{

/**
 * @brief A close message.
 *
 * @details This message is sent from the client to the server to indicate that
 * the client wants to close the connection to the server. Once the server
 * receives this message, it can assume that the client will not send anymore
 * messages and the connection can be closed.
 */
class Close : public Message
{
public:
    /**
     * @brief Construct a close message.
     */
    Close();

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Close(const Close& other) = delete;
    Close& operator=(const Close& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Close(Close&& other) = delete;
    Close& operator=(Close&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the close message.
     */
    ~Close() override = default;

    /**
     * @brief Serialize the message's data into a packet.
     *
     * @param packet The packet to insert the message's data into.
     */
    void serialize(sf::Packet& packet) const override;

    /**
     * @brief Deserialize the message's data from a packet.
     *
     * @param packet The packet to extract the message's data from.
     *
     * @return True if the message was deserialized.
     */
    [[nodiscard]] bool deserialize(sf::Packet& packet) override;
};

}
