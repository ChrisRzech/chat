#pragma once

#include "chat/common/SynchronizedValue.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <optional>

#include <SFML/Network/Packet.hpp>
#include <SFML/Network/TcpSocket.hpp>

namespace chat::server
{

class Connection
{
public:
    explicit Connection(std::unique_ptr<sf::TcpSocket> socket);

    Connection(const Connection& other) = delete;

    Connection& operator=(const Connection& other) = delete;

    Connection(Connection&& other) = delete;

    Connection& operator=(Connection&& other) = delete;

    ~Connection() = default;

    sf::TcpSocket& getSocket();

    bool isBeingHandled() const;

    void setBeingHandled();

    bool isZombie() const;

    void handle();

private:
    std::optional<sf::Packet> receivePacket();
    void sendPacket(sf::Packet packet);

    std::unique_ptr<sf::TcpSocket> m_socket;
    std::atomic_bool m_beingHandled;
    std::atomic_bool m_connected;
    std::atomic_uint32_t m_failCount;
    chat::common::SynchronizedValue<std::chrono::steady_clock::time_point> m_lastUsageTime;
};

}
