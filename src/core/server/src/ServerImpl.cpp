#include "ServerImpl.hpp"

#include "chat/common/Logging.hpp"
#include "chat/common/utility.hpp"

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
    // There needs to be at least 2 threads. A thread is dedicated for socket
    // I/O. The rest are for handling connections.
    if(maxThreadCount < 2) {
        throw std::invalid_argument{
            "unexpected max thread count, expected at least 2"};
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
