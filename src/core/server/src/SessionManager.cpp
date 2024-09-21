#include "SessionManager.hpp"

#include "chat/common/Logging.hpp"

namespace chat::server
{

SessionManager::SessionManager(int maxThreadCount)
  : m_threadPool{maxThreadCount - 1},
    m_selector{},
    m_sessions{}
{}

void SessionManager::add(std::unique_ptr<sf::TcpSocket> socket)
{
    m_selector.add(*socket);
    m_sessions.emplace_back(std::move(socket));
}

void SessionManager::update()
{
    constexpr sf::Int32 SOCKET_READY_TIMEOUT{250};
    if(m_selector.wait(sf::milliseconds(SOCKET_READY_TIMEOUT))) {
        for(auto& session : m_sessions) {
            if(!session.isBeingHandled() && !session.isZombie() &&
               m_selector.isReady(session.getSocket())) {
                // A disconnected socket is also considered ready.

                // This thread must set the session as being handled. If
                // done within the thread pool job, this thread might create
                // multiple thread pool jobs to handle the same message.
                // Imagine if the job never ran and only this thread was
                // running, the session would never be marked as handled
                // and this thread would keep creating jobs.
                session.setBeingHandled();
                m_threadPool.queue([&] { session.handle(); });
            }
        }
    }

    removeZombies();
}

void SessionManager::removeZombies()
{
    for(auto it = m_sessions.begin(); it != m_sessions.end();
        /* update in body */) {
        if(!it->isBeingHandled() && it->isZombie()) {
            LOG_DEBUG << "Cleaning up session";
            auto zombieIt = it++;
            m_selector.remove(zombieIt->getSocket());
            m_sessions.erase(zombieIt);
        } else {
            it++;
        }
    }
}

}
