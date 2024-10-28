#include "ServerImpl.hpp"

#include "chat/common/Logging.hpp"

#include <stdexcept>

namespace chat::server
{
Server::Impl::Impl(std::uint16_t port, int maxThreadCount)
  : m_port{port},
    m_state{State::Stopped},
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
    if(init()) {
        m_state = State::Running;
        LOG_INFO << "Server running";

        while(m_state == State::Running) {
            auto socket = m_listener.accept();
            if(socket.has_value()) {
                m_sessionManager.add(std::move(socket.value()));
            }

            m_sessionManager.update();
        }
    }

    stopping();

    m_state = State::Stopped;
    LOG_INFO << "Server stopped";
}

void Server::Impl::stop()
{
    if(m_state != State::Stopping && m_state != State::Stopped) {
        m_state = State::Stopping;
    }
}

bool Server::Impl::init()
{
    m_state = State::Initializing;
    LOG_INFO << "Server initializing";
    return m_listener.listen(m_port);
}

void Server::Impl::stopping()
{
    m_state = State::Stopping;
    LOG_INFO << "Server stopping";
    m_listener.close();
}
}
