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
    LOG_INFO("Listener: started listening on {}", m_acceptor.local_endpoint());
    startAccept();
}

void Listener::stop()
{
    LOG_INFO("Listener: stopped listening on {}", m_acceptor.local_endpoint());
    m_acceptor.close();
}

void Listener::startAccept()
{
    m_acceptor.listen();
    m_acceptor.async_accept(
        [this](asio::error_code ec, asio::ip::tcp::socket socket) {
            acceptToken(ec, std::move(socket));
        });
}

void Listener::acceptToken(asio::error_code ec, asio::ip::tcp::socket&& socket)
{
    if(ec) {
        LOG_ERROR("Listener: failed to accept, {} ({})",
                  m_acceptor.local_endpoint(), ec.message(), ec.value());
    } else {
        LOG_DEBUG("Listener: accepted connection from {}",
                  socket.remote_endpoint());
        m_connectionManager.start(std::move(socket));
    }

    startAccept();
}
}
