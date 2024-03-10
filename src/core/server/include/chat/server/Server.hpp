#pragma once

#include <cstdint>
#include <memory>

namespace chat::server
{

//TODO Explore the idea of a type that holds all configuration options for the server. The client can also have its own version of this.
/**
 * @brief Chat server.
 *
 * @details The server will wait for clients to connect and process client requests.
 */
class Server
{
public:
    /**
     * @brief Construct a server.
     *
     * @param port The port to listen on.
     *
     * @param maxThreadCount The number of threads for the server to use.
     */
    Server(uint16_t port, uint16_t maxThreadCount);

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Server(const Server& other) = delete;
    Server& operator=(const Server& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Server(Server&& other) = delete;
    Server& operator=(Server&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the server.
     */
    ~Server();

    /**
     * @brief Start the server.
     *
     * @details This does not block.
     */
    void start();

    /**
     * @brief Stop the server.
     *
     * @details This blocks until the server has stopped.
     */
    void stop();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
