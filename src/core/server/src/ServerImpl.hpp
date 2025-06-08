#pragma once

#include "ConnectionManager.hpp"
#include "Listener.hpp"

#include "chat/common/Port.hpp"
#include "chat/common/ThreadPool.hpp"
#include "chat/server/Server.hpp"

#include <asio/io_context.hpp>

#include <atomic>
#include <cstddef>

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
    Impl(common::Port port, std::size_t maxThreadCount);

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
    /**
     * @brief Initialize the server.
     *
     * @return If initialization is successful, true; otherwise, false.
     */
    [[nodiscard]] bool initialize();

    /**
     * @brief Shutdown the server.
     */
    void shutdown();

    std::atomic_bool m_running = false;
    common::ThreadPool m_threadPool;
    ConnectionManager m_connectionManager;
    asio::io_context m_ioContext;
    Listener m_listener;
};
}
