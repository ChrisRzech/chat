#pragma once

#include <sfml/Network/TcpSocket.hpp>

#include <cstdint>
#include <memory>
#include <thread>

namespace chat
{

class ClientConnection
{
public:
    explicit ClientConnection(std::unique_ptr<sf::TcpSocket> clientSocket);

    uint32_t start();

    void stop();

    bool isClosed();

private:
    std::unique_ptr<sf::TcpSocket> m_clientSocket;
    std::thread m_thread;
    bool m_closed;
};

}
