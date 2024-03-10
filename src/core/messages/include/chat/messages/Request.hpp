#pragma once

#include "chat/messages/Message.hpp"

#include <SFML/Network/Packet.hpp>

#include <cstdint>

namespace chat::messages
{

/**
 * @brief A request message.
 *
 * @details A request is sent from the client to the server when the client wants the server to perform an operation. The client should
 * expect a @c Response from the server for each request sent.
 */
class Request : public Message
{
public:
    /**
     * @brief The type of a request.
     */
    enum class Type : uint32_t
    {
        Ping
    };

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Request(const Request& other) = delete;
    Request& operator=(const Request& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Request(Request&& other) = delete;
    Request& operator=(Request&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the request.
     */
    ~Request() override = default;

    /**
     * @brief Get the type of the request.
     *
     * @return The type of the request.
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
     * @brief Construct a request.
     *
     * @param type The type of the request.
     */
    explicit Request(Type type);

private:
    Type m_type;
};

}
