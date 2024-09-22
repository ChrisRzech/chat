#include "ServerImpl.hpp"

#include "chat/common/Logging.hpp"

#include <stdexcept>

namespace chat::server
{

Server::Impl::Impl(std::uint16_t port, int maxThreadCount)
  : m_state{ServerState::Stopped, getServerStateTransitions()},
    m_listener{port},
    m_sessionManager{maxThreadCount}
{
    // There needs to be at least 2 threads, one for the main server thread and
    // one for handling the client requests
    if(maxThreadCount < 2) {
        throw std::invalid_argument{"Max thread count cannot be less than 2"};
    }
}

Server::Impl::~Impl()
{
    stop();
}

void Server::Impl::run()
{
    if(m_state.to(ServerState::Running)) {
        LOG_INFO << "Running";
        while(m_state.get() == ServerState::Running) {
            auto socket = m_listener.accept();
            if(socket.has_value()) {
                m_sessionManager.add(std::move(socket.value()));
            }

            m_sessionManager.update();
        }

        if(m_state.to(ServerState::Stopped)) {
            LOG_INFO << "Stopped";
        }
    }
}

void Server::Impl::stop()
{
    if(m_state.to(ServerState::Stopping)) {
        LOG_INFO << "Stopping";
        m_state.waitUntil(ServerState::Stopped);
    }
}

}
