#pragma once

#include <SFML/Network/IpAddress.hpp>

#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>

namespace chat::client
{

/**
 * @brief Chat client to a chat server.
 *
 * @details The client will attempt to establish a connection to the server if needed before sending requests.
 *
 * @warning A client is not thread-safe.
 */
class Client
{
public:
    /**
     * @brief Construct a client.
     *
     * @param host The address of the chat server to connect to.
     *
     * @param port The port that the chat server is bound to.
     */
    Client(sf::IpAddress host, uint16_t port);

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Client(const Client& other) = delete;
    Client& operator=(const Client& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Client(Client&& other) = delete;
    Client& operator=(Client&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the client.
     */
    ~Client();

    /**
     * @brief Get the elapsed time for making a request and receiving a response.
     *
     * @details The elapsed time measures the round trip time of sending a request and receiving a response.
     *
     * @return The elapsed time for making a request and receiving a response.
     */
    [[nodiscard]] std::optional<std::chrono::milliseconds> ping();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
