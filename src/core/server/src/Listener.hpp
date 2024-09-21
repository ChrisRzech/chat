#pragma once

#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <memory>
#include <optional>

namespace chat::server
{
/**
 * @brief A socket connection listener.
 */
class Listener
{
public:
    /**
     * @brief Construct a listener.
     *
     * @details Once constructed, the listener starts listening.
     *
     * @param port The port to listen on.
     */
    Listener(std::uint16_t port);

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
     * @brief Accept a pending socket connection.
     *
     * @return If a connection was accepted, the socket; otherwise, no value.
     */
    [[nodiscard]] std::optional<std::unique_ptr<sf::TcpSocket>> accept();

private:
    sf::TcpListener m_listener;
};
}
