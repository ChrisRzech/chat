#pragma once

#include "Session.hpp"

#include "chat/common/ThreadPool.hpp"

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpListener.hpp>

#include <cstdint>
#include <list>

namespace chat::server
{

/**
 * @brief A manager for client sessions.
 */
class SessionManager
{
public:
    /**
     * @brief Construct a session manager.
     *
     * @param port The port to listen on.
     *
     * @param maxThreadCount The number of threads to use for managing sessions.
     */
    SessionManager(std::uint16_t port, int maxThreadCount);

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    SessionManager(const SessionManager& other) = delete;
    SessionManager& operator=(const SessionManager& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    SessionManager(SessionManager&& other) = delete;
    SessionManager& operator=(SessionManager&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the manager.
     */
    ~SessionManager() = default;

    void update();

private:
    void accept();

    void removeZombies();

    common::ThreadPool m_threadPool;
    sf::TcpListener m_listener;
    sf::SocketSelector m_selector;
    std::list<Session> m_sessions;
};

}
