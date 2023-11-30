#pragma once

#include "chat/common/SynchronizedValue.hpp"

#include "chat/messages/Response.hpp"
#include "chat/messages/Serializer.hpp"

#include <SFML/Network/Packet.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <atomic>
#include <chrono>
#include <memory>
#include <optional>

namespace chat::messages
{
    class Request;
}

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

    [[nodiscard]] sf::TcpSocket& getSocket();

    [[nodiscard]] bool isBeingHandled() const;

    void setBeingHandled();

    [[nodiscard]] bool isZombie() const;

    void handle();

private:
    [[nodiscard]] std::optional<sf::Packet> receivePacket();
    void sendPacket(sf::Packet& packet);

    [[nodiscard]] std::optional<std::unique_ptr<chat::messages::Request>> receiveRequest();
    void sendResponse(const chat::messages::Response& response);

    std::unique_ptr<sf::TcpSocket> m_socket;
    std::atomic_bool m_beingHandled;
    std::atomic_bool m_connected;
    std::atomic_uint32_t m_failCount;
    chat::common::SynchronizedValue<std::chrono::steady_clock::time_point> m_lastUsageTime;
    chat::messages::Serializer m_serializer;
};

}
