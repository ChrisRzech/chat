#pragma once

#include <sfml/Network/TcpListener.hpp>

#include <cstdint>
#include <list>
#include <memory>
#include <tuple>

namespace chat
{

class ClientConnection;

class ChatServer
{
public:
    explicit ChatServer(uint16_t port);

    uint32_t run();

private:
    std::tuple<bool, std::shared_ptr<ClientConnection>> acceptConnection();

    uint16_t m_port;
    bool m_stopping;
    std::list<std::shared_ptr<ClientConnection>> m_clientConnections;
    sf::TcpListener m_listener;
};

}
