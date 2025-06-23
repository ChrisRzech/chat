#include "ConnectionManager.hpp"

#include "Connection.hpp"

#include "chat/common/ThreadPool.hpp"

#include <asio/ip/tcp.hpp>

#include <memory>
#include <utility>

namespace chat::server
{
ConnectionManager::ConnectionManager(common::ThreadPool& threadPool)
  : m_threadPool{threadPool},
    m_requestHandler{},
    m_connections{}
{}

void ConnectionManager::start(asio::ip::tcp::socket&& socket)
{
    auto connection =
        std::make_shared<Connection>(std::move(socket), *this, m_threadPool);
    connection->start();
    m_connections.emplace_back(std::move(connection));
}

void ConnectionManager::remove(const Connection& connection)
{
    m_connections.remove_if(
        [&connection](const auto& elem) { return elem.get() == &connection; });
}

void ConnectionManager::stopAll()
{
    for(auto& connection : m_connections) {
        connection->stop();
    }
}
}
