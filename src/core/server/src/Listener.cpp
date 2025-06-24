#include "Listener.hpp"

#include "ConnectionManager.hpp"
#include "Formatter.hpp" // NOLINT(misc-include-cleaner)

#include "chat/common/Logging.hpp"

#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

#include <utility>

namespace chat::server
{
Listener::Listener(asio::io_context& ioContext,
                   const asio::ip::tcp::endpoint& endpoint,
                   ConnectionManager& connectionManager)
  : m_acceptor{ioContext, endpoint},
    m_connectionManager{connectionManager}
{}

void Listener::start()
{
    LOG_INFO("Listener: started listening on {}", getEndpoint());
    startAccept();
}

void Listener::stop()
{
    if(!m_acceptor.is_open()) {
        return;
    }

    LOG_INFO("Listener: stopped listening on {}", getEndpoint());
    m_acceptor.close();
}

asio::ip::tcp::endpoint Listener::getEndpoint() const
{
    asio::error_code ec;
    auto endpoint = m_acceptor.local_endpoint(ec);
    if(ec) {
        LOG_WARN("Listener: failed to get local endpoint, {}", ec);
    }

    return endpoint;
}

void Listener::startAccept()
{
    m_acceptor.async_accept(
        [this](asio::error_code ec, asio::ip::tcp::socket socket) {
            acceptToken(ec, std::move(socket));
        });
}

void Listener::acceptToken(asio::error_code ec, asio::ip::tcp::socket&& socket)
{
    if(ec) {
        LOG_WARN("Listener: failed to accept, {}", getEndpoint(), ec);
    } else {
        asio::error_code ec;
        auto remoteEndpoint = socket.remote_endpoint(ec);
        if(ec) {
            LOG_WARN(
                "Listener: failed to get remote endpoint of connection, {}",
                ec);
        }

        LOG_DEBUG("Listener: accepted connection from {}", remoteEndpoint);
        m_connectionManager.start(std::move(socket));
    }

    startAccept();
}
}
