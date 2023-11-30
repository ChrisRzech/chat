#include "Connection.hpp"

#include "chat/common/Logging.hpp"

#include "chat/messages/Ping.hpp"
#include "chat/messages/Pong.hpp"
#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

namespace chat::server
{

Connection::Connection(std::unique_ptr<sf::TcpSocket> socket)
  : m_socket{std::move(socket)},
    m_beingHandled{false},
    m_connected{true},
    m_failCount{0},
    m_lastUsageTime{std::chrono::steady_clock::now()},
    m_serializer{}
{
    /*
    TODO Determine if the socket should be blocking or non-blocking. If the socket is blocking, there should be less send/receive syscalls
    due to not having to deal with partial sends/receives. If the socket is non-blocking, there is no chance of hanging which is crucial to
    the health of the server.
    */
    m_socket->setBlocking(false);
}

sf::TcpSocket& Connection::getSocket()
{
    return *m_socket;
}

bool Connection::isBeingHandled() const
{
    return m_beingHandled;
}

void Connection::setBeingHandled()
{
    m_beingHandled = true;
}

bool Connection::isZombie() const
{
    constexpr uint32_t MAX_FAIL_COUNT = 5;
    constexpr std::chrono::seconds MAX_IDLE_TIME{60}; //TODO A parameter to the server
    const auto now = std::chrono::steady_clock::now();
    return !m_connected || m_failCount > MAX_FAIL_COUNT || now - m_lastUsageTime.lock().get() > MAX_IDLE_TIME;
}

void Connection::handle()
{
    LOG_DEBUG << "Handling connection...";

    {
        auto lockedLastUsageTime = m_lastUsageTime.lock();
        lockedLastUsageTime.get() = std::chrono::steady_clock::now();
    }

    std::optional<std::unique_ptr<chat::messages::Response>> response;
    try
    {
        if(auto request = receiveRequest(); request.has_value())
        {
            switch(request.value()->getType()) //TODO A handler for all derived requests types
            {
            case chat::messages::Request::Type::Ping:
            {
                LOG_DEBUG << "Received test request";
                auto& ping = static_cast<chat::messages::Ping&>(*request.value().get());
                LOG_DEBUG << "Message: '" << ping.getMessage() << "'";
                response = std::make_optional(std::make_unique<chat::messages::Pong>("pong"));
                break;
            }
            }
        }
    }
    catch(const std::exception& exception)
    {
        LOG_ERROR << exception.what();
        response.reset();
    }
    catch(...)
    {
        LOG_ERROR << "Unknown exception!";
        response.reset();
    }

    if(m_connected) //Socket can be disconnected when receiving a request
    {
        if(!response.has_value())
        {
            /*
            TODO Creating an error response. This should probably be done as part of the request handlers since errors might be
            request-specific. Although, if none of the reuqest handlers returned a response, even an error response, this ensures that some
            response is at least sent back.

            There needs to be an overall thought of how errors should be communicated back to the client. Should there be a error code
            provided for a response? Should each derived response type define its own error codes (this might sound good)? The idea of
            using a single error code for all types is not a good idea since many of the values will not be relevant to the response.
            */
            // response = std::make_optional(std::make_unique<chat::messages::Error>());
        }
        sendResponse(*response.value());
    }

    LOG_DEBUG << "Finished handling connection";
    m_beingHandled = false;
}

std::optional<sf::Packet> Connection::receivePacket()
{
    LOG_DEBUG << "Receiving packet...";

    bool success = false;
    sf::Packet packet;
    switch(m_socket->receive(packet))
    {
    case sf::Socket::Status::Done:
        LOG_DEBUG << "Packet received";
        success = true;
        break;

    case sf::Socket::Status::NotReady:
        LOG_WARN << "Could not receive packet, unexpected `sf::Socket::Status::NotReady`";
        m_failCount++;
        break;

    case sf::Socket::Status::Partial:
        LOG_WARN << "Could not receive packet, unexpected `sf::Socket::Status::Partial`";
        m_failCount++;
        break;

    case sf::Socket::Status::Disconnected:
        LOG_DEBUG << "Could not receive packet since the socket is disconnected";
        m_connected = false;
        break;

    case sf::Socket::Status::Error:
        LOG_WARN << "Could not receive packet since an error occurred";
        m_failCount++;
        break;
    }

    LOG_DEBUG << (m_connected ? "Still connected" : "Disconnected") << ". Failed '" << m_failCount << "' times";
    LOG_DEBUG << "Finished receiving packet";

    return success ? std::make_optional(packet) : std::nullopt;
}

void Connection::sendPacket(sf::Packet& packet)
{
    LOG_DEBUG << "Sending packet...";

    switch(m_socket->send(packet))
    {
    case sf::Socket::Status::Done:
        LOG_DEBUG << "Packet sent";
        break;

    case sf::Socket::Status::NotReady:
        LOG_WARN << "Could not send packet, unexpected `sf::Socket::Status::NotReady`";
        m_failCount++;
        break;

    case sf::Socket::Status::Partial:
        LOG_WARN << "Could not send packet, unexpected `sf::Socket::Status::Partial`";
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

    LOG_DEBUG << (m_connected ? "Still connected" : "Disconnected") << ". Failed '" << m_failCount << "' times";
    LOG_DEBUG << "Finished sending packet";
}

std::optional<std::unique_ptr<chat::messages::Request>> Connection::receiveRequest()
{
    LOG_DEBUG << "Receiving request...";

    std::optional<std::unique_ptr<chat::messages::Request>> request;
    if(auto packet = receivePacket(); packet.has_value())
    {
        if(auto message = m_serializer.deserialize(packet.value()); message.has_value())
        {
            /*
            The message is placed inside an `std::unique_ptr`. There is no standard library functionality to transfer ownership from a
            `std::unique_ptr` base type to a `std::unique_ptr` derived type. This must be done manually.
            */
            if(auto temp = dynamic_cast<chat::messages::Request*>(message.value().get()); temp != nullptr)
            {
                message.value().release();
                request = std::make_optional(std::unique_ptr<chat::messages::Request>{temp});
            }
        }
    }

    LOG_DEBUG << "Finished receiving request";
    return request;
}

void Connection::sendResponse(const chat::messages::Response& response)
{
    LOG_DEBUG << "Sending response...";

    sf::Packet packet;
    m_serializer.serialize(response, packet);
    sendPacket(packet);

    LOG_DEBUG << "Finished sending response";
}

}
