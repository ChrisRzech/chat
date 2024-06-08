#include "Session.hpp"

#include "RequestHandler.hpp"

#include "chat/common/Logging.hpp"

#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"
#include "chat/messages/serialize.hpp"

#include "chat/messages/request/Ping.hpp"

#include "chat/messages/response/Pong.hpp"

namespace chat::server
{

Session::Session(std::unique_ptr<sf::TcpSocket> socket)
  : m_socket{std::move(socket)},
    m_beingHandled{false},
    m_connected{true},
    m_failCount{0},
    m_lastUsageTime{std::chrono::steady_clock::now()}
{
    // TODO A custom socket class should be made to allow custom features to it
    // such as blocking non-blocking send/receive and secure communication. A
    // blocking non-blocking send/receive means that the class' send and receive
    // calls are blocking but don't internally use blocking functionality. This
    // allows the use of timeouts and prevents potential deadlocks regarding
    // blocking calls. For secure communication, using OpenSSL is recommended.
    // This would require using third-party libraries which CMake would need to
    // be configured to do so.
    //
    // However, SFML is being used for everything networking related. Splitting
    // up the functionality among different libraries (i.e. this vs SFML) might
    // cause confusion down the line. It's usually better to have the whole
    // functionality come from a central place. While the solution above should
    // be implemented, the implementation could also add wrappers for any
    // networking classes (e.g. socket selector, packet, listener).
    m_socket->setBlocking(false);
}

sf::TcpSocket& Session::getSocket()
{
    return *m_socket;
}

bool Session::isBeingHandled() const
{
    return m_beingHandled;
}

void Session::setBeingHandled()
{
    m_beingHandled = true;
}

bool Session::isZombie() const
{
    constexpr int MAX_FAIL_COUNT = 5;
    constexpr std::chrono::seconds MAX_IDLE_TIME{60};
    const auto now = std::chrono::steady_clock::now();
    auto lockedLastUsageTime = m_lastUsageTime.lock();
    return !m_connected || m_failCount > MAX_FAIL_COUNT ||
           now - lockedLastUsageTime.get() > MAX_IDLE_TIME;
}

void Session::handle()
{
    LOG_DEBUG << "Handling session...";

    {
        auto lockedLastUsageTime = m_lastUsageTime.lock();
        lockedLastUsageTime.get() = std::chrono::steady_clock::now();
    }

    std::optional<std::unique_ptr<messages::Response>> response;
    try {
        auto request = receiveRequest();
        if(request.has_value()) {
            RequestHandler handler;
            response = std::make_optional(handler.handle(*request.value()));
        }
    } catch(const std::exception& exception) {
        LOG_ERROR << exception.what();
        response.reset();
    } catch(...) {
        LOG_ERROR << "Unknown exception!";
        response.reset();
    }

    // Socket can be disconnected when receiving a request
    if(m_connected) {
        if(!response.has_value()) {
            // TODO Creating an error response. This should probably be done as
            // part of the request handlers since errors might be request
            // specific. Although, if none of the reuqest handlers returned a
            // response, even an error response, this ensures that some response
            // is at least sent back.
            //
            // There needs to be an overall thought of how errors should be
            // communicated back to the client. Should there be a error code
            // provided for a response? Should each derived response type define
            // its own error codes (this might sound good)? The idea of using a
            // single error code for all types is not a good idea since many of
            // the values will not be relevant to the response.
            // response =
            // std::make_optional(std::make_unique<messages::Error>());
        }
        sendResponse(*response.value());
    }

    LOG_DEBUG << "Finished handling Session";
    m_beingHandled = false;
}

std::optional<sf::Packet> Session::receivePacket()
{
    LOG_DEBUG << "Receiving packet...";

    bool success = false;
    sf::Packet packet;
    switch(m_socket->receive(packet)) {
    case sf::Socket::Status::Done:
        LOG_DEBUG << "Packet received";
        success = true;
        break;

    case sf::Socket::Status::NotReady:
        LOG_WARN << "Could not receive packet, unexpected "
                    "`sf::Socket::Status::NotReady`";
        m_failCount++;
        break;

    case sf::Socket::Status::Partial:
        LOG_WARN << "Could not receive packet, unexpected "
                    "`sf::Socket::Status::Partial`";
        m_failCount++;
        break;

    case sf::Socket::Status::Disconnected:
        LOG_DEBUG
            << "Could not receive packet since the socket is disconnected";
        m_connected = false;
        break;

    case sf::Socket::Status::Error:
        LOG_WARN << "Could not receive packet since an error occurred";
        m_failCount++;
        break;
    }

    LOG_DEBUG << (m_connected ? "Still connected" : "Disconnected")
              << ". Failed '" << m_failCount << "' times";
    LOG_DEBUG << "Finished receiving packet";

    return success ? std::make_optional(packet) : std::nullopt;
}

void Session::sendPacket(sf::Packet& packet)
{
    LOG_DEBUG << "Sending packet...";

    switch(m_socket->send(packet)) {
    case sf::Socket::Status::Done:
        LOG_DEBUG << "Packet sent";
        break;

    case sf::Socket::Status::NotReady:
        LOG_WARN << "Could not send packet, unexpected "
                    "`sf::Socket::Status::NotReady`";
        m_failCount++;
        break;

    case sf::Socket::Status::Partial:
        LOG_WARN << "Could not send packet, unexpected "
                    "`sf::Socket::Status::Partial`";
        m_failCount++;
        break;

    case sf::Socket::Status::Disconnected:
        LOG_DEBUG << "Could not send packet since the socket is disconnected";
        m_connected = false;
        break;

    case sf::Socket::Status::Error:
        LOG_WARN << "Could not send packet since an error occurred";
        m_failCount++;
        break;
    }

    LOG_DEBUG << (m_connected ? "Still connected" : "Disconnected")
              << ". Failed '" << m_failCount << "' times";
    LOG_DEBUG << "Finished sending packet";
}

std::optional<std::unique_ptr<messages::Request>> Session::receiveRequest()
{
    LOG_DEBUG << "Receiving request...";

    std::optional<std::unique_ptr<messages::Request>> request;
    auto packet = receivePacket();
    if(packet.has_value()) {
        const common::ByteSpan serialized{
            static_cast<const std::byte*>(packet.value().getData()),
            packet.value().getDataSize()};
        auto message = messages::deserialize(serialized);
        if(message.has_value()) {
            // The message is placed inside an `std::unique_ptr`. There is no
            // standard library functionality to transfer ownership from a
            // `std::unique_ptr` base type to a `std::unique_ptr` derived type.
            // This must be done manually.
            if(message.value()->getType() == messages::Message::Type::Request) {
                request = std::make_optional(std::unique_ptr<messages::Request>(
                    dynamic_cast<messages::Request*>(
                        message.value().release())));
            } else {
                LOG_ERROR << "Received unexpected request type";
            }
        }
    }

    LOG_DEBUG << "Finished receiving request";
    return request;
}

void Session::sendResponse(const messages::Response& response)
{
    LOG_DEBUG << "Sending response...";

    auto serialized = messages::serialize(response);
    sf::Packet packet;
    packet.append(serialized.data(), serialized.size());
    sendPacket(packet);

    LOG_DEBUG << "Finished sending response";
}

}
