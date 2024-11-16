#pragma once

#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include <SFML/Network/Packet.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <atomic>
#include <memory>
#include <optional>

namespace chat::server
{
/**
 * @brief A communication channel to the client.
 *
 * @details A session handles receiving requests from and sending responses to
 * the client.
 *
 * The server follows a request-response model. The server receives a request
 * from the client, creates a response, and sends the response to the client.
 */
class Session
{
public:
    /**
     * @brief Construct a session.
     *
     * @param socket The socket used to communicate with the client.
     */
    explicit Session(std::unique_ptr<sf::TcpSocket> socket);

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Session(const Session& other) = delete;
    Session& operator=(const Session& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Session(Session&& other) = delete;
    Session& operator=(Session&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the session.
     */
    ~Session() = default;

    /**
     * @brief Check if the session is in the disconnected state.
     *
     * @return True if the session is in the disconnected state; otherwise,
     * false.
     */
    [[nodiscard]] bool isDisconnected() const;

    /**
     * @brief Get the socket used for the session.
     *
     * @return The socket used for the session.
     */
    [[nodiscard]] sf::TcpSocket& getSocket();

    /**
     * @brief Try to receive a request.
     *
     * @details A request may be partially received. If so, this function should
     * be called again until a full request is received.
     *
     * If the session is not in the @c State::Receiving state, this function
     * does nothing.
     *
     * If a request is received, the session transitions to the
     * @c State::Handling state.
     *
     * If the socket has disconnected, the session transitions to the
     * @c State::Disconnected state.
     *
     * @return If a full request is received, a request; otherwise, no value.
     */
    [[nodiscard]] std::optional<std::unique_ptr<messages::Request>>
    tryReceive();

    /**
     * @brief Set the response to send.
     *
     * @details If the session is not in the @c State::Handling state, this
     * function does nothing.
     *
     * The session transitions to the @c State::Sending state.
     *
     * @param response The response to send.
     */
    void setResponse(const messages::Response& response);

    /**
     * @brief Try to send the response.
     *
     * @details The response may be partially sent. If so, this function should
     * be called again until the full response is sent.
     *
     * If the session is not in the @c State::Sending state, this function does
     * nothing.
     *
     * If the response is sent, the session transitions to the
     * @c State::Receiving state.
     *
     * If the socket has disconnected, the session transitions to the
     * @c State::Disconnected state.
     */
    void trySend();

private:
    /**
     * @brief The states of the session.
     *
     * @details In the @c State::Receiving state, the session is waiting for a
     * request to be received.
     *
     * In the @c State::Handling state, the session is waiting for the request
     * to be handled and a response to be set.
     *
     * In the @c State::Sending state, the session is sending a response.
     *
     * In the @c State::Disconnected state, the session is disconnected and
     * should no longer be used.
     */
    enum class State
    {
        Receiving,
        Handling,
        Sending,
        Disconnected
    };

    State m_state;
    std::unique_ptr<sf::TcpSocket> m_socket;
    sf::Packet m_receivingPacket;
    sf::Packet m_sendingPacket;
};
}
