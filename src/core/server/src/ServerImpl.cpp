#include "ServerImpl.hpp"

#include "chat/common/Logging.hpp"
#include "chat/common/Port.hpp"
#include "chat/common/utility.hpp"
#include "chat/server/Server.hpp"

#include <asio/ip/tcp.hpp>

#include <cstddef>
#include <stdexcept>

namespace chat::server
{
namespace
{
auto createListenerEndpoint(common::Port port)
{
    return asio::ip::tcp::endpoint{asio::ip::tcp::v4(),
                                   common::utility::toUnderlying(port)};
}
}

Server::Impl::Impl(common::Port port, std::size_t maxThreadCount)
  : m_running{false},
    m_threadPool{maxThreadCount},
    m_connectionManager{m_threadPool},
    m_ioContext{},
    m_listener{m_ioContext, createListenerEndpoint(port), m_connectionManager}
{
    if(maxThreadCount < 1) {
        throw std::invalid_argument{"max thread count must greater than 0"};
    }
}

void Server::Impl::run()
{
    if(initialize()) {
        LOG_INFO("Server online");
        m_running = true;
        m_ioContext.run();
    }

    shutdown();
    LOG_INFO("Server offline");
}

void Server::Impl::stop()
{
    if(m_running) {
        m_running = false;
        m_ioContext.stop();
    }
}

bool Server::Impl::initialize()
{
    LOG_INFO("Server initializing");
    m_listener.start();
    return true;
}

void Server::Impl::shutdown()
{
    LOG_INFO("Server shutting down");
    m_listener.stop();
    m_connectionManager.stopAll();
}
}
