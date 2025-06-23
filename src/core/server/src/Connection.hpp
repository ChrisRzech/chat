#pragma once

#include "chat/common/Buffer.hpp"
#include "chat/common/BufferView.hpp"
#include "chat/common/FixedBuffer.hpp"
#include "chat/common/Synced.hpp"
#include "chat/common/ThreadPool.hpp"
#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include <asio/ip/tcp.hpp>

#include <memory>

namespace chat::server
{
// Forward declare to break circular dependency between `ConnectionManager.hpp`
// and `Connection.hpp`
class ConnectionManager;

/**
 * @brief A communication channel to the client.
 *
 * @details A connection is the middleman between the client and server. It
 * manages the I/O operations to receive data from the client and send data to
 * the client. The received data is passed to a handler, which processes the
 * data and sends data back to the client through the connection.
 *
 * The connection utilizes a 2-stage buffer system for receiving, handling, and
 * sending data. The following is a diagram to visualize the flow of data:
 *            +---------+      +---------+
 *            | stage 1 |      | stage 2 |
 *      +---> | receive | ---> | receive | -----+
 *      |     | buffer  |      | buffer  |      |
 *      |     +---------+      +---------+      v
 * +--------+                              +---------+
 * | socket |                              | handler |
 * +--------+                              +---------+
 *      ^     +---------+      +---------+      |
 *      |     | stage 2 |      | stage 1 |      |
 *      +---- | send    | <--- | send    | <----+
 *            | buffer  |      | buffer  |
 *            +---------+      +---------+
 *
 * Transferring data between stage 1 and stage 2 buffers is done in a
 * thread-safe manner since the socket and handler could be running at the same
 * time.
 *
 * The lifetime of a connection is managed through `std::shared_ptr`s and
 * `std::enable_shared_from_this`. To use `shared_from_this()`, an
 * `std::shared_ptr` must already exist before it can be called, which is done
 * by the connection manager upon creating the connection object when accepted
 * by the listener. The result of `shared_from_this()` is passed to thread pool
 * jobs and the completion tokens to prevent destroying the connection while
 * one of them is outstanding.
 */
class Connection : public std::enable_shared_from_this<Connection>
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
     */
    void stop();

private:
    /**
     * @brief Set the remote endpoint.
     */
    void setRemoteEndpoint();

    /**
     * @brief Start the asynchronous receive operation.
     *
     * @details The received data is placed into the stage 1 receive buffer.
     */
    void startReceive();

    /**
     * @brief The completion token for the asynchronous receive operation.
     *
     * @details If an error is indicated, the connection is stopped.
     *
     * The data in the stage 1 receive buffer is transferred into the stage 2
     * receive buffer. If a job isn't currently running or the current job is
     * about to stop running, a new job is queued to handle the data in the
     * stage 2 receive buffer.
     *
     * The asynchronous operation is started again.
     *
     * @param ec The error code from the asynchronous operation.
     * @param bytesReceived The number of bytes received.
     */
    void receiveToken(asio::error_code ec, std::size_t bytesReceived);

    /**
     * @brief Handle received data until there is no more.
     *
     * @details The data in the stage 2 receive buffer is extracted from the
     * buffer and is handled. This repeats until there is no more data to
     * handle.
     */
    void handleReceivedDataLoop();

    /**
     * @brief Handle received data.
     *
     * @details The received data is consumed. Depending on the logic, data may
     * be added to the stage 1 send buffer to be sent to the client.
     *
     * @param data The received data.
     */
    void handleReceivedData(common::Buffer data);

    /**
     * @brief Send data to the client.
     *
     * @details The data is inserted into the stage 1 send buffer. If an
     * asynchronous send operation isn't currently running, it is started.
     *
     * @param data The data to send.
     */
    void send(common::BufferView data);

    /**
     * @brief Start the asynchronous send operation.
     *
     * @details The data from the stage 1 send buffer is transferred into the
     * stage 2 send buffer. If there is data in the stage 2 send buffer, the
     * asynchronous send operation is started using the stage 2 send buffer.
     */
    void startSend();

    /**
     * @brief The completion token for the asynchronous send operation.
     *
     * @details If an error is indicated, the connection is stopped.
     *
     * The bytes sent in the stage 2 send buffer are deleted from the buffer.
     *
     * The asynchronous operation is started again.
     *
     * @param ec The error code from the asynchronous operation.
     * @param bytesSent The number of bytes sent.
     */
    void sendToken(asio::error_code ec, std::size_t bytesSent);

    /**
     * @brief Transfer the data in the receive buffers from stage 1 to stage 2.
     *
     * @details The receive data job stops executing once there is no more data
     * in the stage 2 receive buffer. The return value of this function is used
     * to determine if a new job should be queued.
     *
     * @param bytesReceived The number of bytes in the stage 1 receive buffer,
     * starting from the start of the buffer, to transfer into the stage 2
     * receive buffer.
     *
     * @return True if the stage 2 receive buffer was empty before the transfer;
     * false otherwise.
     */
    bool transferReceiveBuffers(std::size_t bytesReceived);

    /**
     * @brief Extract all the data from the stage 2 receive buffer.
     *
     * @return The data from the stage 2 receive buffer.
     */
    common::Buffer extractReceiveBufferStage2();

    /**
     * @brief Insert data into the stage 1 send buffer.
     *
     * @param data The data to insert into the stage 1 send buffer.
     *
     * @return True if the stage 1 send buffer was empty before the insertion;
     * false otherwise.
     */
    bool insertSendBufferStage1(common::BufferView data);

    /**
     * @brief Transfer the data in the send buffers from stage 1 to stage 2.
     */
    void transferSendBuffers();

    static constexpr std::size_t receiveBufferStage1Size = 256;

    asio::ip::tcp::socket m_socket;
    ConnectionManager& m_connectionManager;
    common::ThreadPool& m_threadPool;
    asio::ip::tcp::endpoint m_remoteEndpoint;
    common::FixedBuffer<receiveBufferStage1Size> m_receiveBufferStage1;
    common::Synced<common::Buffer> m_receiveBufferStage2;
    common::Synced<common::Buffer> m_sendBufferStage1;
    common::Buffer m_sendBufferStage2;
};
}
