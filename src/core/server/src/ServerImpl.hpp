#pragma once

#include "Listener.hpp"
#include "ServerState.hpp"
#include "SessionManager.hpp"

#include "chat/server/Server.hpp"

#include <atomic>
#include <cstdint>

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
    ~Impl() = default;

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
     * @brief The states of the server.
     *
     * @details In the @c Initializing state, the server performs operations
     * needed before going online.
     *
     * In the @c Online state, the server accepts incoming connections and
     * processes client requests.
     *
     * In the @c Shutdown state, the server performs operations needed before
     * going offline.
     *
     * to the @c Offline state, the server can't accept incoming connections.
     */
    enum class State
    {
        Initializing,
        Online,
        Shutdown,
        Offline
    };

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

    std::uint16_t m_port;
    std::atomic<State> m_state;
    Listener m_listener;
    SessionManager m_sessionManager;
};
}
