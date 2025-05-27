#pragma once

#include "chat/common/Buffer.hpp"
#include "chat/common/BufferView.hpp"
#include "chat/common/FixedBuffer.hpp"
#include "chat/common/ThreadPool.hpp"

#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include <asio/ip/tcp.hpp>

namespace chat::server
{
// Forward declare to break circular dependency between `ConnectionManager.hpp`
// and `Connection.hpp`
class ConnectionManager;

/**
 * @brief A communication channel to the client.
 *
 * @details A connection is essentially the middleman between the business logic
 * of the server and the client. It manages the I/O operations to receive data
 * from and send data to the client.
 *
 * When data is received, @c handleReceiveJob() is queued with the data into the
 * thread pool.
 *
 * TODO: Need to document how is data sent back to the client? `m_sending` needs
 * to be populated and `startSend()` needs to be called, but who is doing both
 * of these? It might be safe to assume that `handleReceiveJob()` will
 * essentially end up being a request handler that can generate a response. The
 * response would then be serialized and fill up `m_sending` and then call
 * `startSend()`. Up until this point, everything has been single-threaded. With
 * `handleReceiveJob()` being done in a thread, modifying `m_sending` should be
 * done in a thread-safe manner.
 *
 * In Asio, I/O objects (e.g. `asio::ip::tcp::sockets`) must outlive their
 * asynchronous operations (e.g. `async_receive()`). Typically, to meet this
 * requirement, some implementations use `shared_from_this()` to provide each
 * asynchronous operation completion token an `std::shared_ptr<Connection>`.
 *
 * However, it is assumed that the server's design uses a single thread for
 * managing I/O operations. The controller manager controls the lifetime of a
 * connection. So before a connection is destroyed, `close()` is called on its
 * socket, which stops all outstanding asynchronous operations and their
 * completion token is called. Therefore, there is no need to use
 * `std::shared_ptr` here.
 */
class Connection
{
public:
    /**
     * @brief Construct a connection.
     *
     * @param socket The socket used to communicate with the client.
     * @param connectionManager The manager for this connection.
     * @param threadPool The thread pool to queue work into to handle received
     * data.
     */
    Connection(asio::ip::tcp::socket&& socket,
               ConnectionManager& connectionManager,
               common::ThreadPool& threadPool);

    /**
     * @brief Start the connection.
     *
     * @details The connection starts receiving data from the client.
     */
    void start();

    /**
     * @brief Stop the connection.
     *
     * @details The socket's asynchronous operations are cancelled and the
     * connection destroyed.
     *
     * Since this connection object is destroyed during this function, the
     * object should not be used after calling this.
     */
    void stop();

private:
    /**
     * @brief Start the asynchronous receive operation.
     */
    void startReceive();

    /**
     * @brief The completion token for the asynchronous receive operation.
     *
     * @param ec The error code from the asynchronous operation.
     * @param bytesReceived The number of bytes received.
     */
    void receiveToken(asio::error_code ec, std::size_t bytesReceived);

    /**
     * @brief Start the asynchronous send operation.
     */
    void startSend();

    /**
     * @brief The completion token for the asynchronous send operation.
     *
     * @param ec The error code from the asynchronous operation.
     * @param bytesSent The number of bytes sent.
     */
    void sendToken(asio::error_code ec, std::size_t bytesSent);

    /**
     * @brief The handler for data received from the client.
     *
     * @param data The data to handle.
     */
    void handleReceiveJob(common::Buffer data);

    /**
     * @brief Add data to the back of the sending data buffer.
     *
     * @param data The data to add.
     */
    void enqueueSendingBuffer(common::BufferView data);

    /**
     * @brief Remove data from the front of the sending data buffer.
     *
     * @param bytesSent The number of bytes sent.
     */
    void dequeueSendingBuffer(std::size_t bytesSent);

    static constexpr std::size_t receiveBufferSize = 256;

    asio::ip::tcp::socket m_socket;
    ConnectionManager& m_connectionManager;
    common::ThreadPool& m_threadPool;
    common::FixedBuffer<receiveBufferSize> m_receiving;
    common::Buffer m_sending;
};
}
