#include "ClientConnection.hpp"

#include <sfml/Network/TcpSocket.hpp>

#include <cstdint>
#include <memory>
#include <thread>

namespace chat
{

ClientConnection::ClientConnection(std::unique_ptr<sf::TcpSocket> clientSocket)
    : m_clientSocket{std::move(clientSocket)}
{}

uint32_t ClientConnection::start()
{
    //m_thread = std::thread{*this};

    return 0;
}

void ClientConnection::stop()
{
    //Join request handler thread
    //Send closing message
    //Disconnect socket
}

bool ClientConnection::isClosed()
{
    return m_closed;
}

}
