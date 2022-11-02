#include "chat/chatserver/ChatServer.hpp"

#include "ClientConnection.hpp"

#include <sfml/Network/SocketSelector.hpp>
#include <sfml/Network/TcpListener.hpp>
#include <sfml/Network/TcpSocket.hpp>

#include <cstdint>
#include <list>
#include <memory>
#include <tuple>

#include <iostream>

namespace chat
{

ChatServer::ChatServer(uint16_t port)
  : m_port{port},
    m_stopping{false}
{
    m_listener.setBlocking(false);
    m_listener.listen(m_port);
}

uint32_t ChatServer::run()
{
    std::cout << "Server running..." << std::endl;
    while(!m_stopping)
    {
        auto [connected, clientConnection] = acceptConnection();
        if(connected)
        {
            clientConnection->start();
        }
    }
    std::cout << "Server stopped" << std::endl;

    return 0;
}

std::tuple<bool, std::shared_ptr<ClientConnection>> ChatServer::acceptConnection()
{
    std::unique_ptr<sf::TcpSocket> clientSocket = std::make_unique<sf::TcpSocket>();
    switch(m_listener.accept(*clientSocket))
    {
        case sf::Socket::Status::Done:
        {
            auto clientConnection = std::make_shared<ClientConnection>(std::move(clientSocket));
            m_clientConnections.emplace_back(clientConnection);
            return std::make_pair(true, std::move(clientConnection));
            break;
        }

        case sf::Socket::Status::NotReady:
            break;

        case sf::Socket::Status::Partial:
            break;

        case sf::Socket::Status::Disconnected:
            break;

        case sf::Socket::Status::Error:
            break;
    }

    return std::make_pair(false, std::shared_ptr<ClientConnection>{});
}

}
