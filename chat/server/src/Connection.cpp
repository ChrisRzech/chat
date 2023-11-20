#include "Connection.hpp"

#include "chat/common/Logging.hpp"

#include "chat/messages/Message.hpp"
#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"
#include "chat/messages/TestRequest.hpp"
#include "chat/messages/TestResponse.hpp"

namespace chat::server
{

namespace
{

std::optional<std::unique_ptr<chat::messages::Request>> constructRequest(sf::Packet& packet)
{
    /*
    The message is placed inside an `std::unique_ptr` when constructed from the packet. A message contains a polymorphic type, one of which
    is a request. Since the server is expecting a request, this message must be downcasted appropriately. However, since `std::unique_ptr`
    is used, there is no equivalent for `std::dynamic_pointer_cast`. Therefore, the downcast must be done differently. This is done by
    taking the raw pointer to the message and downcasting it into a request. After error checking, the ownership of the object is transfered
    into a new `std::unique_ptr` for requests.

    https://stackoverflow.com/questions/17417848/stdunique-ptr-with-derived-class
    */
    auto message = chat::messages::Message::createFromPacket(packet);
    auto temp = dynamic_cast<chat::messages::Request*>(message.get());
    if(temp != nullptr)
    {
        message.release();
        //std::make_unique() is not used here because requests are an abstract type
        return std::make_optional(std::unique_ptr<chat::messages::Request>{temp});
    }
    else
    {
        return std::nullopt;
    }
}

}

Connection::Connection(std::unique_ptr<sf::TcpSocket> socket)
  : m_socket{std::move(socket)},
    m_beingHandled{false},
    m_connected{true},
    m_failCount{0},
    m_lastUsageTime{std::chrono::steady_clock::now()}
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

    std::optional<std::unique_ptr<chat::messages::Response>> response;
    try
    {
        if(auto packet = receivePacket(); packet.has_value())
        {
            LOG_DEBUG << "Received packet";
            if(auto request = constructRequest(packet.value()); request.has_value())
            {
                LOG_DEBUG << "Received valid request";
                //TODO Need a request handler class that implements handlers for all request types.
                switch(request.value()->getType())
                {
                case chat::messages::Request::Type::Test:
                {
                    LOG_DEBUG << "Received test request";
                    auto& testRequest = static_cast<chat::messages::TestRequest&>(*request.value().get());
                    LOG_DEBUG << "Message: '" << testRequest.getMessage() << "'";
                    response = std::make_optional(std::make_unique<chat::messages::TestResponse>("test response"));
                    break;
                }
                }
            }
            else
            {
                LOG_DEBUG << "Received invalid request";
            }
        }
        else
        {
            LOG_DEBUG << "Received invalid packet";
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

    if(!response.has_value())
    {
        //TODO Create error response
        //response = std::make_optional(std::make_unique<chat::messages::Error>());
    }

    sendPacket(response.value()->toPacket());

    LOG_DEBUG << "Finished handling connection";
    m_beingHandled = false;
}

std::optional<sf::Packet> Connection::receivePacket()
{
    LOG_DEBUG << "Receiving packet...";

    {
        auto lockedLastUsageTime = m_lastUsageTime.lock();
        lockedLastUsageTime.get() = std::chrono::steady_clock::now();
    }

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
        LOG_WARN << "Could not receive packet since the socket is disconnected";
        m_connected = false;
        break;

    case sf::Socket::Status::Error:
        LOG_WARN << "Could not receive packet since an error occurred";
        m_failCount++;
        break;
    }

    LOG_DEBUG << "This connection is " << (m_connected ? "still connected" : "disconnected");
    LOG_DEBUG << "This connection has failed '" << m_failCount << "' times";
    LOG_DEBUG << "Finished receiving packet";

    return success ? std::make_optional(packet) : std::nullopt;
}

void Connection::sendPacket(sf::Packet packet)
{
    LOG_DEBUG << "Sending packet..";

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
        LOG_WARN << "Could not send packet since the socket is disconnected";
        m_connected = false;
        break;

    case sf::Socket::Status::Error:
        LOG_WARN << "Could not send packet since an error occurred";
        m_failCount++;
        break;
    }

    LOG_DEBUG << "This connection is " << (m_connected ? "still connected" : "disconnected");
    LOG_DEBUG << "This connection has failed '" << m_failCount << "' times";
    LOG_DEBUG << "Finished sending packet";
}

}
