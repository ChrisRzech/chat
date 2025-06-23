#pragma once

#include "Connection.hpp"
#include "RequestHandler.hpp"

#include "chat/common/ThreadPool.hpp"

#include <asio/ip/tcp.hpp>

#include <list>
#include <memory>

namespace chat::server
{
/**
 * @brief A manager for connections.
 *
 * @details A connection manager simply knows about all connections at any given
 * moment. It is the one that creates and destroys them. All connections are
 * kept in an internal list.
 */
class ConnectionManager
{
public:
    /**
     * @brief Construct a connection manager.
     *
     * @param threadPool The thread pool to pass to connections.
     */
    explicit ConnectionManager(common::ThreadPool& threadPool);

    /**
     * @brief Create and start a new connection.
     *
     * @param socket The socket for the connection.
     */
    void start(asio::ip::tcp::socket&& socket);

    /**
     * @brief Remove connection from the manager.
     *
     * @param connection The connection to remove.
     */
    void remove(const Connection& connection);

    /**
     * @brief Stop and remove all connections.
     */
    void stopAll();

private:
    common::ThreadPool& m_threadPool;
    RequestHandler m_requestHandler;
    std::list<std::shared_ptr<Connection>> m_connections;
};
}
