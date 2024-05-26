#include "chat/client/Client.hpp"

#include "chat/common/Logging.hpp"

#include "chat/messages/serialize.hpp"

#include "chat/messages/request/Ping.hpp"

#include "chat/messages/response/Pong.hpp"

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/TcpSocket.hpp>

namespace chat::client
{

class Client::Impl
{
public:
    Impl(const std::string& host, std::uint16_t port)
      : m_host{host},
        m_port{port},
        m_socket{},
        m_connected{false}
    {
        m_socket.setBlocking(true);
    }

    [[nodiscard]] std::optional<std::chrono::milliseconds> ping()
    {
        LOG_DEBUG << "Sending ping...";

        //`sendAndReceive()` is not used here so that establishing a connection
        // is not included in the elapsed time measurement

        if(!m_connected && !connect()) {
            return std::nullopt;
        }

        std::optional<std::chrono::milliseconds> result;
        auto start = std::chrono::system_clock::now();
        if(sendRequest(messages::Ping{})) {
            if(receiveResponse<messages::Pong>().has_value()) {
                auto end = std::chrono::system_clock::now();
                result = std::make_optional(
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        end - start));
            }
        }

        LOG_DEBUG << "Finished ping";
        return result;
    }

private:
    [[nodiscard]] bool connect()
    {
        LOG_DEBUG << "Connecting to host...";

        bool success = false;

        // The `connect()` call will disconnect the socket before reconnecting,
        // assume it is disconnected
        m_connected = false;
        switch(m_socket.connect(m_host, m_port)) {
        case sf::Socket::Status::Done:
            LOG_DEBUG << "Connected to host";
            m_connected = true;
            success = true;
            break;

        case sf::Socket::Status::NotReady:
            LOG_WARN << "Could not connect to host, unexpected "
                        "`sf::Socket::Status::NotReady`";
            break;

        case sf::Socket::Status::Partial:
            LOG_WARN << "Could not connect to host, unexpected "
                        "`sf::Socket::Status::Partial`";
            break;

        case sf::Socket::Status::Disconnected:
            LOG_WARN << "Could not connect to host, unexpected "
                        "`sf::Socket::Status::Disconnected`";
            break;

        case sf::Socket::Status::Error:
            LOG_WARN << "An error occured while trying to connect to host";
            break;
        }

        LOG_DEBUG << "Finished connecting to host";
        return success;
    }

    [[nodiscard]] bool sendPacket(sf::Packet& packet)
    {
        LOG_DEBUG << "Sending packet...";

        bool success = false;
        switch(m_socket.send(packet)) {
        case sf::Socket::Status::Done:
            LOG_DEBUG << "Packet sent";
            success = true;
            break;

        case sf::Socket::Status::NotReady:
            LOG_WARN << "Could not send request, unexpected "
                        "`sf::Socket::Status::NotReady`";
            break;

        case sf::Socket::Status::Partial:
            LOG_WARN << "Could not send request, unexpected "
                        "`sf::Socket::Status::Partial`";
            break;

        case sf::Socket::Status::Disconnected:
            // This should not happen:
            // https://stackoverflow.com/a/14782354/21445636
            LOG_WARN << "Could not send request, unexpected "
                        "`sf::Socket::Status::Disconnect`";
            m_connected = false;
            break;

        case sf::Socket::Status::Error:
            LOG_WARN << "An error occured while trying to send request";
            break;
        }

        LOG_DEBUG << "Finished sending packet";
        return success;
    }

    [[nodiscard]] std::optional<sf::Packet> receivePacket()
    {
        LOG_DEBUG << "Receiving packet...";

        bool success = false;
        sf::Packet packet;
        switch(m_socket.receive(packet)) {
        case sf::Socket::Status::Done:
            LOG_DEBUG << "Packet received";
            success = true;
            break;

        case sf::Socket::Status::NotReady:
            LOG_WARN << "Could not receive request, unexpected "
                        "`sf::Socket::Status::NotReady`";
            break;

        case sf::Socket::Status::Partial:
            LOG_WARN << "Could not receive request, unexpected "
                        "`sf::Socket::Status::Partial`";
            break;

        case sf::Socket::Status::Disconnected:
            LOG_WARN << "Could not receive request since the socket is "
                        "disconnected";
            m_connected = false;
            break;

        case sf::Socket::Status::Error:
            LOG_WARN << "An error occured while trying to receive request";
            break;
        }

        LOG_DEBUG << "Finished receiving packet";
        return success ? std::make_optional(packet) : std::nullopt;
    }

    [[nodiscard]] bool sendRequest(const messages::Request& request)
    {
        LOG_DEBUG << "Sending request...";

        auto serialized = messages::serialize(request);
        sf::Packet packet;
        packet.append(serialized.data(), serialized.size());
        const bool success = sendPacket(packet);

        LOG_DEBUG << "Finished sending request";
        return success;
    }

    template<typename ResponseType>
    [[nodiscard]] std::optional<std::unique_ptr<ResponseType>> receiveResponse()
    {
        static_assert(std::is_base_of_v<messages::Response, ResponseType>,
                      "Response is not a base of ResponseType");

        LOG_DEBUG << "Receiving response...";

        std::optional<std::unique_ptr<ResponseType>> response;
        if(auto packet = receivePacket(); packet.has_value()) {
            const common::ByteSpan serialized{
                static_cast<const std::byte*>(packet.value().getData()),
                packet.value().getDataSize()};
            if(auto message = messages::deserialize(serialized);
               message.has_value()) {
                // The message is placed inside an `std::unique_ptr`. There is
                // no standard library functionality to transfer ownership from
                // a `std::unique_ptr` base type to a `std::unique_ptr` derived
                // type. This must be done manually.
                if(message.value()->getType() ==
                   messages::Message::Type::Response) {
                    response = std::make_optional(std::unique_ptr<ResponseType>(
                        dynamic_cast<ResponseType*>(
                            message.value().release())));
                } else {
                    LOG_ERROR << "Received unexpected response type";
                }
            }
        }

        LOG_DEBUG << "Finished receiving response";
        return response;
    }

    template<typename RequestType, typename ResponseType,
             typename... RequestArgs>
    [[nodiscard]] std::optional<std::unique_ptr<ResponseType>> sendAndReceive(
        RequestArgs&&... args)
    {
        if(!m_connected && !connect()) {
            return std::nullopt;
        }

        if(!sendRequest(RequestType{std::forward<RequestArgs>(args)...})) {
            return std::nullopt;
        }

        return receiveResponse<ResponseType>();
    }

    sf::IpAddress m_host;
    std::uint16_t m_port;
    sf::TcpSocket m_socket;
    bool m_connected;
};

Client::Client(const std::string& host, std::uint16_t port)
  : m_impl{std::make_unique<Impl>(host, port)}
{}

Client::~Client() = default;

std::optional<std::chrono::milliseconds> Client::ping()
{
    return m_impl->ping();
}

}
