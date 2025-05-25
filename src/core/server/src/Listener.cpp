#include "Listener.hpp"

#include "chat/common/Logging.hpp"

namespace chat::server
{
Listener::Listener(asio::io_context& ioContext,
                   asio::ip::tcp::endpoint endpoint,
                   ConnectionManager& connectionManager)
  : m_ioContext{ioContext},
    m_acceptor{ioContext, endpoint},
    m_connectionManager{connectionManager}
{}

void Listener::start()
{
    LOG_INFO << "Listener (" << m_acceptor.local_endpoint()
             << "): started listening";
    startAccept();
}

void Listener::stop()
{
    LOG_INFO << "Listener (" << m_acceptor.local_endpoint()
             << "): stopped listening";
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
    if(!ec) {
        LOG_DEBUG << "Listener (" << m_acceptor.local_endpoint()
                  << "): accepted connection from " << socket.remote_endpoint();
        m_connectionManager.start(std::move(socket));
    } else {
        LOG_ERROR << "Listener (" << m_acceptor.local_endpoint()
                  << "): failed to accept, " << ec.message() << " ("
                  << ec.value() << ")";
    }

    startAccept();
}
}
