#include "ConnectionManager.hpp"

#include "chat/common/Logging.hpp"

namespace chat::server
{
ConnectionManager::ConnectionManager(common::ThreadPool& threadPool)
  : m_threadPool{threadPool},
    m_requestHandler{},
    m_connections{}
{}

void ConnectionManager::start(asio::ip::tcp::socket&& socket)
{
    m_connections.emplace_back(std::move(socket), *this, m_threadPool).start();
}

void ConnectionManager::remove(Connection& connection)
{
    m_connections.remove_if(
        [&connection](auto& elem) { return &elem == &connection; });
}

void ConnectionManager::stopAll()
{
    for(auto& connection : m_connections) {
        connection.stop();
    }
}
}
