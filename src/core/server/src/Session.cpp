#include "Session.hpp"

#include "chat/common/Logging.hpp"

#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"
#include "chat/messages/serialize.hpp"

namespace chat::server
{
namespace
{
std::optional<std::unique_ptr<messages::Request>> deserializeRequest(
    const sf::Packet& packet)
{
    std::optional<std::unique_ptr<messages::Request>> request;

    const common::ByteSpan serialized{
        static_cast<const std::byte*>(packet.getData()), packet.getDataSize()};
    auto message = messages::deserialize(serialized);
    if(message.has_value()) {
        // The message is placed inside an `std::unique_ptr`. There is no
        // standard library functionality to transfer ownership from a
        // `std::unique_ptr` base type to a `std::unique_ptr` derived type.
        // This must be done manually.
        if(message.value()->getMessageType() ==
           messages::Message::Type::Request) {
            request = std::unique_ptr<messages::Request>(
                dynamic_cast<messages::Request*>(message.value().release()));
        } else {
            LOG_WARN << "Received non-request message type";
        }
    }

    return request;
}
}

Session::Session(std::unique_ptr<sf::TcpSocket> socket)
  : m_state{State::Receiving},
    m_socket{std::move(socket)},
    m_receivingPacket{},
    m_sendingPacket{}
{
    m_socket->setBlocking(false);
}

bool Session::isDisconnected() const
{
    return m_state == State::Disconnected;
}

sf::TcpSocket& Session::getSocket()
{
    return *m_socket;
}

std::optional<std::unique_ptr<messages::Request>> Session::tryReceive()
{
    std::optional<std::unique_ptr<messages::Request>> request;

    if(m_state == State::Receiving) {
        switch(m_socket->receive(m_receivingPacket)) {
        case sf::Socket::Status::Done:
            LOG_DEBUG << "Received full packet";
            m_state = State::Handling;
            request = deserializeRequest(m_receivingPacket);
            m_receivingPacket.clear();
            break;

        case sf::Socket::Status::NotReady:
            LOG_WARN << "Failed to receive packet: unexpected not ready";
            break;

        case sf::Socket::Status::Partial:
            LOG_DEBUG << "Received partial packet";
            break;

        case sf::Socket::Status::Disconnected:
            LOG_DEBUG << "Failed to receive packet: disconnected";
            m_state = State::Disconnected;
            break;

        case sf::Socket::Status::Error:
            LOG_WARN << "Failed to receive packet: unexpected error";
            break;
        }
    }

    return request;
}

void Session::setResponse(const messages::Response& response)
{
    if(m_state == State::Handling) {
        LOG_DEBUG << "Response set";
        m_state = State::Sending;
        auto serialized = messages::serialize(response);
        m_sendingPacket.append(serialized.data(), serialized.size());
    }
}

void Session::trySend()
{
    if(m_state == State::Sending) {
        switch(m_socket->send(m_sendingPacket)) {
        case sf::Socket::Status::Done:
            LOG_DEBUG << "Sent full packet";
            m_state = State::Receiving;
            m_sendingPacket.clear();
            break;

        case sf::Socket::Status::NotReady:
            LOG_WARN << "Failed to send packet: unexpected not ready";
            break;

        case sf::Socket::Status::Partial:
            LOG_DEBUG << "Sent partial packet";
            break;

        case sf::Socket::Status::Disconnected:
            LOG_DEBUG << "Failed to send packet: disconnected";
            m_state = State::Disconnected;
            break;

        case sf::Socket::Status::Error:
            LOG_WARN << "Failed to send packet: unexpected error";
            break;
        }
    }
}
}
