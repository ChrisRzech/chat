#include "Listener.hpp"

#include "chat/common/Logging.hpp"

namespace chat::server
{
Listener::Listener()
  : m_listener{}
{
    m_listener.setBlocking(false);
}

bool Listener::listen(std::uint16_t port)
{
    bool successful = false;

    switch(m_listener.listen(port)) {
    case sf::Socket::Status::Done:
        LOG_INFO << "Listening for connections on port '" << port << "'";
        successful = true;
        break;

    case sf::Socket::Status::NotReady:
        LOG_ERROR << "Failed to listen on port '" << port
                  << "': unexpected not ready";
        break;

    case sf::Socket::Status::Partial:
        LOG_ERROR << "Failed to listen on port '" << port
                  << "': unexpected partial";
        break;

    case sf::Socket::Status::Disconnected:
        LOG_ERROR << "Failed to listen on port '" << port
                  << "': unexpected disconnected";
        break;

    case sf::Socket::Status::Error:
        LOG_ERROR << "Failed to listen on port '" << port
                  << "': unexpected error";
        break;
    }

    return successful;
}

std::optional<std::unique_ptr<sf::TcpSocket>> Listener::accept()
{
    std::optional<std::unique_ptr<sf::TcpSocket>> result;

    auto socket = std::make_unique<sf::TcpSocket>();
    switch(m_listener.accept(*socket)) {
    case sf::Socket::Status::Done:
        LOG_INFO << "Accepted connection from '" << socket->getRemoteAddress()
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

void Listener::close()
{
    m_listener.close();
}
}
