#pragma once

#include "ConnectionManager.hpp"

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

namespace chat::server
{
/**
 * @brief Listen for incoming connections.
 */
class Listener
{
public:
    /**
     * @brief Construct a listener.
     *
     * @param ioContext The execution context for I/O.
     * @param endpoint The endpoint to listen for connections on.
     * @param connectionManager The manager to create connections from.
     */
    Listener(asio::io_context& ioContext,
             const asio::ip::tcp::endpoint& endpoint,
             ConnectionManager& connectionManager);

    /**
     * @brief Start listening for connections.
     */
    void start();

    /**
     * @brief Stop listening for connections.
     */
    void stop();

private:
    void startAccept();
    void acceptToken(asio::error_code ec, asio::ip::tcp::socket&& socket);

    asio::ip::tcp::acceptor m_acceptor;
    ConnectionManager& m_connectionManager;
};
}
