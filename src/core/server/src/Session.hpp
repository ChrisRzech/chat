#pragma once

#include "chat/common/SynchronizedObject.hpp"

#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include <SFML/Network/Packet.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <atomic>
#include <chrono>
#include <memory>
#include <optional>

namespace chat::server
{

/**
 * @brief A client session.
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
     * @brief Get the socket used to communicate with the client.
     *
     * @return The socket used to communicate with the client.
     */
    [[nodiscard]] sf::TcpSocket& getSocket();

    /**
     * @brief Check if the session is currently being handled.
     *
     * @return True if the session is currently being handled; otherwise,
     * false.
     */
    [[nodiscard]] bool isBeingHandled() const;

    /**
     * @brief Mark the session as currently being handled.
     */
    void setBeingHandled();

    /**
     * @brief Check if the session is a zombie.
     *
     * @details A zombie session should be removed as they are no longer used
     * or should be used.
     *
     * @return True if the session is a zombie; otherwise, false.
     */
    [[nodiscard]] bool isZombie() const;

    /**
     * @brief Handle incoming requests from the socket.
     */
    void handle();

private:
    /**
     * @brief Receive a packet from the socket.
     *
     * @return A packet from the socket. No value if an error occurred.
     */
    [[nodiscard]] std::optional<sf::Packet> receivePacket();

    /**
     * @brief Send a packet through the socket.
     *
     * @param packet The packet to send through the socket.
     */
    void sendPacket(sf::Packet& packet);

    /**
     * @brief Receive a request message from the socket.
     *
     * @return A request message from the socket. No value if an error occurred.
     */
    [[nodiscard]] std::optional<std::unique_ptr<messages::Request>>
    receiveRequest();

    /**
     * @brief Send a response message through the socket.
     *
     * @param response The response message to send through the socket.
     */
    void sendResponse(const messages::Response& response);

    std::unique_ptr<sf::TcpSocket> m_socket;
    std::atomic_bool m_beingHandled;
    std::atomic_bool m_connected;
    common::SynchronizedObject<std::chrono::steady_clock::time_point>
        m_lastUsageTime;
};

}
