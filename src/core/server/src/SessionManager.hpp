#pragma once

#include "Session.hpp"

#include "chat/common/ThreadPool.hpp"

#include <SFML/Network/SocketSelector.hpp>

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
     * @param maxThreadCount The number of threads to use for managing sessions.
     */
    SessionManager(int maxThreadCount);

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

    /**
     * @brief Add a new session.
     *
     * @param socket The socket for the session.
     */
    void add(std::unique_ptr<sf::TcpSocket> socket);

    void update();

private:
    void accept();

    void removeZombies();

    common::ThreadPool m_threadPool;
    sf::SocketSelector m_selector;
    std::list<Session> m_sessions;
};

}
