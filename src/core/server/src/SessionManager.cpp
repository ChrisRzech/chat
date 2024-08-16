#include "SessionManager.hpp"

#include "chat/common/Logging.hpp"

namespace chat::server
{

SessionManager::SessionManager(std::uint16_t port, int maxThreadCount)
  : m_threadPool{maxThreadCount - 1},
    m_listener{},
    m_selector{},
    m_sessions{}
{
    m_listener.listen(port);
    m_selector.add(m_listener);
}

void SessionManager::update()
{
    constexpr sf::Int32 SOCKET_READY_TIMEOUT{250};
    if(m_selector.wait(sf::milliseconds(SOCKET_READY_TIMEOUT))) {
        if(m_selector.isReady(m_listener)) {
            accept();
        }

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

void SessionManager::accept()
{
    auto socket = std::make_unique<sf::TcpSocket>();
    switch(m_listener.accept(*socket)) {
    case sf::Socket::Status::Done:
        LOG_INFO << "Session accepted";
        m_selector.add(*socket);
        m_sessions.emplace_back(std::move(socket));
        break;

    case sf::Socket::Status::NotReady:
        LOG_WARN << "Failed to accept socket: unexpected not ready";
        break;

    case sf::Socket::Status::Partial:
        LOG_WARN << "Failed to accept socket: unexpected partial";
        break;

    case sf::Socket::Status::Disconnected:
        LOG_WARN << "Failed to accept socket: unexpected disconnected";
        break;

    case sf::Socket::Status::Error:
        LOG_WARN << "An error occurred while trying to accept socket";
        break;
    }
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
