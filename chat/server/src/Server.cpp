#include "chat/server/Server.hpp"

#include "Connection.hpp"

#include "chat/common/Logging.hpp"
#include "chat/common/ThreadPool.hpp"

#include <atomic>
#include <cstdint>
#include <list>
#include <memory>
#include <stdexcept>
#include <thread>

#include <SFML/Network/Packet.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>

/*
(OUTDATED)
The server should be doing the following concurrently:
1. Accept incoming connections (if possible, may have an arbitrary limit of current connections)
    * Before accepting the incoming connection, the client and server must exchange information about each other and set up
        the environment (e.g. TLS handshake for secure communications)
2. Receive request messages from any connected socket and putting them into the request queue
3. Send reply messages to any connected socket
4. Service any pending request message and place reply messages into the reply queue
    * As a result of servicing a request message, a notification may need to be sent out. If this is the case, the notification
        message is put into a notification queue.
5. Service any pending notification message to be sent out to clients who need to be notified.
*/

namespace chat::server
{

class Server::Impl
{
public:
    Impl(uint16_t port, uint16_t maxThreadCount)
      : m_port{port},
        m_maxThreadCount{maxThreadCount},
        m_stopping{false},
        m_serverThread{}
    {
        //There needs to be at least 2 threads, one for the main server thread and one for handling the client requests
        if(maxThreadCount < 2)
        {
            LOG_FATAL << "Max thread count cannot be less than 2";
            throw std::invalid_argument{"Max thread count cannot be less than 2"};
        }
    }

    ~Impl()
    {
        stop();
    }

    void start()
    {
        LOG_INFO << "Server starting...";
        m_serverThread = std::thread{run, this};
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

        chat::common:: ThreadPool threadPool{static_cast<uint16_t>(m_maxThreadCount - 1)}; //Count this thread towards the number of threads
        threadPool.start();

        while(!m_stopping)
        {
            if(socketSelector.wait(sf::milliseconds(250)))
            {
                /*
                TODO Determine if working on a single or multiple sockets per `wait()` call is better performance. It might depend on the
                server traffic. If so, should the server switch between the two when it can predict if traffic will be heavy. It might not
                even matter that much.
                */

                if(socketSelector.isReady(listener))
                {
                    listen(listener, connections, socketSelector);
                }

                for(auto& connection : connections)
                {
                    if(!connection.isBeingHandled() && !connection.isZombie() && socketSelector.isReady(connection.getSocket()))
                    {
                        /*
                        TODO Is it possible that the client is disconnected and still pass `isReady()`? It might not be necessary anymore
                        since the sockets have a timeout time anyway, but it would be nice to remove the sockets as soon as possible.

                        After researching, a Unix poll() call will say that a socket is ready when its disconnected, meaning that a
                        receive() call will return 0 even though the poll() said it was ready. In addition, the SFML code seems to indicate
                        a disconnect when a similar case happens. Therefore, it is safe to say that the disconnect will be seen before the
                        timeout (depending on the timeout time).
                        */
                        /*
                        This thread must set the connection as being handled. If done within the thread pool job, this thread might create
                        multiple thread pool jobs to handle the same message. Imagine if the job never ran and only this thread was running,
                        the connection would never be marked as handled and this thread would keep creating jobs.
                        */
                        connection.setBeingHandled();
                        threadPool.queue([&connection]{connection.handle();});
                    }
                }
            }

            cleanupConnections(connections, socketSelector);
        }

        threadPool.stop();
    }

    void listen(sf::TcpListener& listener, std::list<Connection>& connections, sf::SocketSelector& socketSelector)
    {
        LOG_DEBUG << "Listening for connection...";

        auto socket = std::make_unique<sf::TcpSocket>();
        switch(listener.accept(*socket))
        {
        case sf::Socket::Status::Done:
            LOG_DEBUG << "Socket accepted";

            socketSelector.add(*socket);
            connections.emplace_back(std::move(socket));
            break;

        case sf::Socket::Status::NotReady:
            LOG_WARN << "Could not accept socket, unexpected `sf::Socket::Status::NotReady`";
            break;

        case sf::Socket::Status::Partial:
            LOG_WARN << "Could not accept socket, unexpected `sf::Socket::Status::Partial`";
            break;

        case sf::Socket::Status::Disconnected:
            LOG_WARN << "Could not accept socket, unexpected `sf::Socket::Status::Disconnected`";
            break;

        case sf::Socket::Status::Error:
            LOG_WARN << "An error occurred while trying to accept socket";
            break;
        }

        LOG_DEBUG << "Finished listening for connection";
    }

    void cleanupConnections(std::list<Connection>& connections, sf::SocketSelector& socketSelector)
    {
        for(auto it = connections.begin(); it != connections.end(); /* update in body */)
        {
            if(!it->isBeingHandled() && it->isZombie())
            {
                LOG_DEBUG << "Cleaning up connection...";

                auto zombieIt = it++;
                socketSelector.remove(zombieIt->getSocket());
                connections.erase(zombieIt);

                LOG_DEBUG << "Finished cleaning up connection";
            }
            else
            {
                it++;
            }
        }
    }

    uint16_t m_port;
    uint16_t m_maxThreadCount;
    std::atomic_bool m_stopping;
    std::thread m_serverThread;
};

Server::Server(uint16_t port, uint16_t maxThreadCount)
  : impl{std::make_unique<Impl>(port, maxThreadCount)}
{}

Server::~Server() = default;

void Server::start()
{
    impl->start();
}

void Server::stop()
{
    impl->stop();
}

}
