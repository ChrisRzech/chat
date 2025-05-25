#include "ServerImpl.hpp"

#include "chat/common/Logging.hpp"
#include "chat/common/utility.hpp"

#include <stdexcept>

namespace chat::server
{
Server::Impl::Impl(common::Port port, std::size_t maxThreadCount)
  : m_port{port},
    m_state{State::Offline},
    m_listener{},
    m_sessionManager{maxThreadCount}
{
    // There needs to be at least 2 threads: one for the server loop and one for
    // handling client requests
    if(maxThreadCount < 2) {
        throw std::invalid_argument{"Max thread count cannot be less than 2"};
    }
}

void Server::Impl::run()
{
    if(initialize()) {
        m_state = State::Online;
        LOG_INFO << "Server online";

        while(m_state == State::Online) {
            auto socket = m_listener.accept();
            if(socket.has_value()) {
                m_sessionManager.add(std::move(socket.value()));
            }

            m_sessionManager.update();
        }
    }

    shutdown();

    m_state = State::Offline;
    LOG_INFO << "Server offline";
}

void Server::Impl::stop()
{
    if(m_state != State::Shutdown && m_state != State::Offline) {
        m_state = State::Shutdown;
    }
}

bool Server::Impl::initialize()
{
    m_state = State::Initializing;
    LOG_INFO << "Server initializing";
    return m_listener.listen(common::utility::toUnderlying(m_port));
}

void Server::Impl::shutdown()
{
    m_state = State::Shutdown;
    LOG_INFO << "Server shutting down";
    m_listener.close();
}
}
