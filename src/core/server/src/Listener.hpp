#pragma once

#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <memory>
#include <optional>

namespace chat::server
{
/**
 * @brief A socket that accepts incoming connections.
 */
class Listener
{
public:
    /**
     * @brief Construct a listener.
     */
    Listener();

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Listener(const Listener& other) = delete;
    Listener& operator=(const Listener& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Listener(Listener&& other) = delete;
    Listener& operator=(Listener&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the listener.
     */
    ~Listener() = default;

    /**
     * @brief Listen for incoming connections on a port.
     *
     * @param port The port to listen on.
     */
    [[nodiscard]] bool listen(std::uint16_t port);

    /**
     * @brief Accept an incoming connection.
     *
     * @details The listener must be listening on a port in order to accept
     * connections.
     *
     * @return If a connection was accepted, the socket; otherwise, no value.
     */
    [[nodiscard]] std::optional<std::unique_ptr<sf::TcpSocket>> accept();

    /**
     * @brief Stop listening for incoming connections.
     */
    void close();

private:
    sf::TcpListener m_listener;
};
}
