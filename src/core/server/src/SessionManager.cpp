#include "SessionManager.hpp"

#include "chat/common/Logging.hpp"

namespace chat::server
{
SessionManager::SessionManager(int maxThreadCount)
  : m_threadPool{maxThreadCount - 1},
    m_requestHandler{},
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
    tryReceives();
    trySends();
    tryRemoves();
}

void SessionManager::tryReceives()
{
    // TODO There is a latency issue with this design. When receiving requests,
    // the connections are polled using an arbitrary timeout. However, this
    // needs to finish before a response can be sent. This adds latency for
    // sending responses.
    //
    // A different design would be to have a thread dedicated to sending
    // responses. When the handler has finished, this thread is notified that a
    // response is ready to be sent back.
    //
    // For now, an arbitrarily small timeout value reduces the latency while
    // reducing CPU usage due to loop frequency.

    constexpr sf::Int32 pollTimeout{10};
    if(m_selector.wait(sf::milliseconds(pollTimeout))) {
        for(auto& session : m_sessions) {
            if(m_selector.isReady(session.getSocket())) {
                auto request = session.tryReceive();
                if(request.has_value()) {
                    m_threadPool.queue([&] {
                        auto response =
                            m_requestHandler.handle(*request.value());
                        session.setResponse(*response);
                    });
                }
            }
        }
    }
}

void SessionManager::trySends()
{
    for(auto& session : m_sessions) {
        session.trySend();
    }
}

void SessionManager::tryRemoves()
{
    for(auto it = m_sessions.begin(); it != m_sessions.end();
        /* update in body */) {
        if(it->isDisconnected()) {
            LOG_DEBUG << "Removing disconnected session";
            auto removeIt = it++;
            m_selector.remove(removeIt->getSocket());
            m_sessions.erase(removeIt);
        } else {
            it++;
        }
    }
}
}
