#pragma once

#include "Connection.hpp"
#include "StateManager.hpp"

#include "chat/server/Server.hpp"

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpListener.hpp>

#include <cstdint>
#include <list>

namespace chat::server
{

/**
 * @brief Implementation for @c chat::server::Server.
 */
class Server::Impl
{
public:
    /**
     * @brief Construct a server.
     *
     * @param port The port to listen on.
     *
     * @param maxThreadCount The number of threads for the server to use.
     */
    Impl(std::uint16_t port, int maxThreadCount);

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Impl(const Impl& other) = delete;
    Impl& operator=(const Impl& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Impl(Impl&& other) = delete;
    Impl& operator=(Impl&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the server.
     */
    ~Impl();

    /**
     * @brief Run the server.
     *
     * @details This blocks until the server is stopped.
     */
    void run();

    /**
     * @brief Stop the server.
     *
     * @details This blocks until the server has stopped.
     */
    void stop();

private:
    void doRun();

    void listen(sf::TcpListener& listener, std::list<Connection>& connections,
                sf::SocketSelector& socketSelector);

    void cleanupConnections(std::list<Connection>& connections,
                            sf::SocketSelector& socketSelector);

    std::uint16_t m_port;
    int m_maxThreadCount;
    StateManager m_state;
};

}
