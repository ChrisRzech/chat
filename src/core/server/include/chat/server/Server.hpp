#pragma once

#include "chat/common/Port.hpp"

#include <cstddef>
#include <memory>

namespace chat::server
{
/**
 * @brief Chat server.
 *
 * @details The server will wait for clients to connect and process client
 * requests.
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
    Server(common::Port port, std::size_t maxThreadCount);

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
     * @brief Run the server.
     *
     * @details This blocks until the server is stopped. Use @c stop() to stop
     * the server. Since this function blocks the current thread, @c stop() must
     * be called on a separate thread.
     */
    void run();

    /**
     * @brief Notify the server to stop.
     */
    void stop();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};
}
