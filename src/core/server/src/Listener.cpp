#include "Listener.hpp"

#include "chat/common/Logging.hpp"

namespace chat::server
{
Listener::Listener(std::uint16_t port)
  : m_listener{}
{
    m_listener.setBlocking(false);
    m_listener.listen(port);
}

std::optional<std::unique_ptr<sf::TcpSocket>> Listener::accept()
{
    std::optional<std::unique_ptr<sf::TcpSocket>> result;

    auto socket = std::make_unique<sf::TcpSocket>();
    switch(m_listener.accept(*socket)) {
    case sf::Socket::Status::Done:
        LOG_INFO << "Client connected from '" << socket->getRemoteAddress()
                 << "'";
        result.emplace(std::move(socket));
        break;

    case sf::Socket::Status::NotReady:
        // No pending connection
        break;

    case sf::Socket::Status::Partial:
        LOG_WARN << "Failed to accept socket: unexpected partial";
        break;

    case sf::Socket::Status::Disconnected:
        LOG_WARN << "Failed to accept socket: unexpected disconnected";
        break;

    case sf::Socket::Status::Error:
        LOG_WARN << "An error occurred while trying to accept socket";
        break;
    }

    return result;
}
}
