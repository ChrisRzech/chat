#include "ServerImpl.hpp"

#include "Connection.hpp"

#include "chat/common/Logging.hpp"
#include "chat/common/ThreadPool.hpp"

#include <SFML/Network/TcpSocket.hpp>

#include <memory>
#include <stdexcept>

namespace chat::server
{

Server::Impl::Impl(std::uint16_t port, int maxThreadCount)
  : m_port{port},
    m_maxThreadCount{maxThreadCount},
    m_state{}
{
    // There needs to be at least 2 threads, one for the main server thread and
    // one for handling the client requests
    if(maxThreadCount < 2) {
        LOG_FATAL << "Max thread count cannot be less than 2";
        throw std::invalid_argument{"Max thread count cannot be less than 2"};
    }
}

Server::Impl::~Impl()
{
    stop();
}

void Server::Impl::run()
{
    if(m_state.to(StateManager::State::Running)) {
        LOG_INFO << "Running";
        doRun();
        if(m_state.to(StateManager::State::Stopped)) {
            LOG_INFO << "Stopped";
        }
    }
}

void Server::Impl::stop()
{
    if(m_state.to(StateManager::State::Stopping)) {
        LOG_INFO << "Stopping";
        m_state.waitUntil(StateManager::State::Stopped);
    }
}

void Server::Impl::doRun()
{
    std::list<Connection> connections;

    sf::TcpListener listener;
    listener.listen(m_port);

    sf::SocketSelector socketSelector;
    socketSelector.add(listener);

    // Count this thread towards the number of threads
    common::ThreadPool threadPool{m_maxThreadCount - 1};

    while(m_state.get() == StateManager::State::Running) {
        constexpr sf::Int32 SOCKET_READY_TIMEOUT{250};
        if(socketSelector.wait(sf::milliseconds(SOCKET_READY_TIMEOUT))) {
            if(socketSelector.isReady(listener)) {
                listen(listener, connections, socketSelector);
            }

            for(auto& connection : connections) {
                if(!connection.isBeingHandled() && !connection.isZombie() &&
                   socketSelector.isReady(connection.getSocket())) {
                    // The selector considers a socket "ready" if a socket is
                    // disconnected (i.e. receiving data would indicate socket
                    // is disconnected)

                    // This thread must set the connection as being handled. If
                    // done within the thread pool job, this thread might create
                    // multiple thread pool jobs to handle the same message.
                    // Imagine if the job never ran and only this thread was
                    // running, the connection would never be marked as handled
                    // and this thread would keep creating jobs.
                    connection.setBeingHandled();
                    threadPool.queue([&] { connection.handle(); });
                }
            }
        }

        cleanupConnections(connections, socketSelector);
    }
}

void Server::Impl::listen(sf::TcpListener& listener,
                          std::list<Connection>& connections,
                          sf::SocketSelector& socketSelector)
{
    auto socket = std::make_unique<sf::TcpSocket>();
    switch(listener.accept(*socket)) {
    case sf::Socket::Status::Done:
        LOG_INFO << "Connection accepted";
        socketSelector.add(*socket);
        connections.emplace_back(std::move(socket));
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

void Server::Impl::cleanupConnections(std::list<Connection>& connections,
                                      sf::SocketSelector& socketSelector)
{
    for(auto it = connections.begin(); it != connections.end();
        /* update in body */) {
        if(!it->isBeingHandled() && it->isZombie()) {
            LOG_DEBUG << "Cleaning up connection";
            auto zombieIt = it++;
            socketSelector.remove(zombieIt->getSocket());
            connections.erase(zombieIt);
        } else {
            it++;
        }
    }
}
}
