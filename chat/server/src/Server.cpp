#include "chat/server/Server.hpp"

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
(OUTDATED) The server should be doing the following concurrently:
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
        m_stopping{false},
        m_serverThread{},
        m_threadPool{static_cast<uint16_t>(maxThreadCount - 1)} //Count the server thread towards the number of threads
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
        sf::TcpListener listener;
        std::list<std::shared_ptr<sf::TcpSocket>> sockets;
        sf::SocketSelector socketSelector;

        listener.listen(m_port);
        socketSelector.add(listener);

        m_threadPool.start();

        while(!m_stopping)
        {
            if(socketSelector.wait(sf::milliseconds(250)))
            {
                /*
                TODO Determine if working on a single or multiple sockets per `wait()` call is better performance. It might depend on the
                server traffic. If so, should the server switch between the two when it can predict if traffic will be heavy. It might not
                even matter that much.
                */

                /*
                TODO Disconnected sockets needs to be cleaned up (remove from the selector and list) at some point.

                One way would be to clean up when a `sf::Socket::Status::Disconnect` value is returned when performing a `receive()` from
                the socket. However, it is unsure if this value would be returned since the selector's `wait()`.

                Another way could be to periodically clean up after some time. This might be a problem cause it is not easy to test whether
                a socket is disconnected without trying to send or receive data.

                Another way is to have a timeout for each socket. This way there is no need to check if the socket is disconnected or not
                since the socket will be cleaned up once it hasn't been used for a while.

                A combination of these would be that when a `sf::Socket::Status::Disconnect` is seen, mark the socket as disconnected. Then
                when going through the sockets when checking if they're ready from the socket selector, clean up the socket if it is marked.
                At the same time, idle sockets after an arbitrary time will be cleaned up.
                */

                if(socketSelector.isReady(listener))
                {
                    listen(listener, sockets, socketSelector);
                }

                for(auto& socket : sockets)
                {
                    if(socketSelector.isReady(*socket))
                    {
                        /*
                        TODO Is it possible that the client is disconnected and still pass `isReady()`? This is important to know because
                        if it can, then the socket handler would end up cleaning up the socket from any containers.
                        */

                        /*
                        TODO Maybe the whole thing should be on a thread pool job. The only thing is that information should be kept about
                        the socket during its whole lifetime such as an idle time, connection status, etc. as well as additional logic for
                        handling disconnected sockets and synchronization of sockets.
                        */
                        socketHandler(socket);
                    }
                }
            }
        }

        m_threadPool.stop();
    }

    void listen(sf::TcpListener& listener, std::list<std::shared_ptr<sf::TcpSocket>>& sockets, sf::SocketSelector& socketSelector)
    {
        LOG_DEBUG << "Listening for connection...";

        auto socket = std::make_shared<sf::TcpSocket>();
        switch(listener.accept(*socket))
        {
        case sf::Socket::Status::Done:
        {
            LOG_DEBUG << "Socket accepted";

            socket->setBlocking(false); //TODO Is this necessary if a socket selector is being used? Feels like it doesn't need to be.
            socketSelector.add(*socket);
            sockets.emplace_back(std::move(socket));
            break;
        }

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

    void socketHandler(std::shared_ptr<sf::TcpSocket>& socket)
    {
        LOG_DEBUG << "Handling socket...";

        sf::Packet packet;
        switch(socket->receive(packet))
        {
        case sf::Socket::Status::Done:
            LOG_DEBUG << "Packet received";
            m_threadPool.queue([this, packet = std::move(packet)]{packetHandler(packet);});
            break;

        case sf::Socket::Status::NotReady:
            LOG_WARN << "Could not read packet from socket, unexpected `sf::Socket::Status::NotReady`";
            break;

        case sf::Socket::Status::Partial:
            LOG_WARN << "Could not read packet from socket, unexpected `sf::Socket::Status::Partial`";
            break;

        case sf::Socket::Status::Disconnected:
            LOG_WARN << "Could not read packet from socket since the socket is disconnected";
            //TODO Handle disconnected socket
            break;

        case sf::Socket::Status::Error:
            LOG_WARN << "An error occurred while trying to read packet from socket";
            //TODO Treat this as a disconnect? Maybe we can retry a certain times by keeping track of how many times this happens.
            break;
        }

        LOG_DEBUG << "Finished handling socket";
    }

    void packetHandler(const sf::Packet& packet)
    {
        //TODO Maybe a class called `PacketHandler` which would break down the packet into a request and pass it to a `RequestHandler`?
        LOG_DEBUG << "Handling packet...";

        std::ignore = packet;

        LOG_DEBUG << "Finished handling packet";
    }

    uint16_t m_port;
    std::atomic_bool m_stopping;
    std::thread m_serverThread;
    chat::common::ThreadPool m_threadPool;
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
