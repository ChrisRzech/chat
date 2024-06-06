#include "chat/server/Server.hpp"

#include "ServerImpl.hpp"

namespace chat::server
{

Server::Server(std::uint16_t port, int maxThreadCount)
  : m_impl{std::make_unique<Impl>(port, maxThreadCount)}
{}

Server::~Server() = default;

void Server::run()
{
    m_impl->run();
}

void Server::stop()
{
    m_impl->stop();
}

}
