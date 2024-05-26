#include "chat/server/Server.hpp"

#include "Connection.hpp"

#include "chat/common/Logging.hpp"
#include "chat/common/ThreadPool.hpp"

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <atomic>
#include <cstdint>
#include <list>
#include <memory>
#include <stdexcept>
#include <thread>

namespace chat::server
{

class Server::Impl
{
public:
    Impl(std::uint16_t port, int maxThreadCount)
      : m_port{port},
        m_maxThreadCount{maxThreadCount},
        m_stopping{false},
        m_serverThread{}
    {
        // There needs to be at least 2 threads, one for the main server thread
        // and one for handling the client requests
        if(maxThreadCount < 2) {
            LOG_FATAL << "Max thread count cannot be less than 2";
            throw std::invalid_argument{
                "Max thread count cannot be less than 2"};
        }
    }

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

    ~Impl()
    {
        stop();
    }

    void start()
    {
        LOG_INFO << "Server starting...";
        m_serverThread = std::thread{&Server::Impl::run, this};
        LOG_INFO << "Server started";
    }

    void stop()
    {
        LOG_INFO << "Server stopping...";
        m_stopping = true;
        m_serverThread.join();
        LOG_INFO << "Server stopped";
    }

private:
    void run()
    {
        std::list<Connection> connections;

        sf::TcpListener listener;
        listener.listen(m_port);

        sf::SocketSelector socketSelector;
        socketSelector.add(listener);

        // Count this thread towards the number of threads
        chat::common::ThreadPool threadPool{m_maxThreadCount - 1};

        while(!m_stopping) {
            constexpr sf::Int32 SOCKET_READY_TIMEOUT{250};
            if(socketSelector.wait(sf::milliseconds(SOCKET_READY_TIMEOUT))) {
                if(socketSelector.isReady(listener)) {
                    listen(listener, connections, socketSelector);
                }

                for(auto& connection : connections) {
                    if(!connection.isBeingHandled() && !connection.isZombie() &&
                       socketSelector.isReady(connection.getSocket())) {
                        // The selector considers a socket "ready" if a socket
                        // is disconnected (i.e. receiving data would indicate
                        // socket is disconnected).
                        //
                        // This thread must set the connection as being handled.
                        // If done within the thread pool job, this thread might
                        // create multiple thread pool jobs to handle the same
                        // message. Imagine if the job never ran and only this
                        // thread was running, the connection would never be
                        // marked as handled and this thread would keep creating
                        // jobs.
                        connection.setBeingHandled();
                        threadPool.queue(
                            [&connection] { connection.handle(); });
                    }
                }
            }

            cleanupConnections(connections, socketSelector);
        }

        threadPool.waitForCompletion();
    }

    void listen(sf::TcpListener& listener, std::list<Connection>& connections,
                sf::SocketSelector& socketSelector)
    {
        LOG_DEBUG << "Listening for connection...";

        auto socket = std::make_unique<sf::TcpSocket>();
        switch(listener.accept(*socket)) {
        case sf::Socket::Status::Done:
            LOG_INFO << "Connection accepted";

            socketSelector.add(*socket);
            connections.emplace_back(std::move(socket));
            break;

        case sf::Socket::Status::NotReady:
            LOG_WARN << "Could not accept socket, unexpected "
                        "`sf::Socket::Status::NotReady`";
            break;

        case sf::Socket::Status::Partial:
            LOG_WARN << "Could not accept socket, unexpected "
                        "`sf::Socket::Status::Partial`";
            break;

        case sf::Socket::Status::Disconnected:
            LOG_WARN << "Could not accept socket, unexpected "
                        "`sf::Socket::Status::Disconnected`";
            break;

        case sf::Socket::Status::Error:
            LOG_WARN << "An error occurred while trying to accept socket";
            break;
        }

        LOG_DEBUG << "Finished listening for connection";
    }

    void cleanupConnections(std::list<Connection>& connections,
                            sf::SocketSelector& socketSelector)
    {
        for(auto it = connections.begin(); it != connections.end();
            /* update in body */) {
            if(!it->isBeingHandled() && it->isZombie()) {
                LOG_DEBUG << "Cleaning up connection...";

                auto zombieIt = it++;
                socketSelector.remove(zombieIt->getSocket());
                connections.erase(zombieIt);

                LOG_DEBUG << "Finished cleaning up connection";
            } else {
                it++;
            }
        }
    }

    std::uint16_t m_port;
    int m_maxThreadCount;
    std::atomic_bool m_stopping;
    std::thread m_serverThread;
};

Server::Server(std::uint16_t port, int maxThreadCount)
  : m_impl{std::make_unique<Impl>(port, maxThreadCount)}
{}

Server::~Server() = default;

void Server::start()
{
    m_impl->start();
}

void Server::stop()
{
    m_impl->stop();
}

}
