#include "Connection.hpp"

#include "ConnectionManager.hpp"
#include "Formatter.hpp" // NOLINT(misc-include-cleaner)

#include "chat/common/Logging.hpp"
#include "chat/common/ThreadPool.hpp"

#include <asio/buffer.hpp>
#include <asio/error_code.hpp>
#include <asio/ip/tcp.hpp>

#include <cstddef>
#include <sstream>
#include <string>
#include <utility>

// TODO: Remove includes
#include "chat/common/utility.hpp"

namespace chat::server
{
Connection::Connection(asio::ip::tcp::socket&& socket,
                       ConnectionManager& connectionManager,
                       common::ThreadPool& threadPool)
  : m_socket{std::move(socket)},
    m_connectionManager{connectionManager},
    m_threadPool{threadPool},
    m_remoteEndpoint{},
    m_receiveBufferStage1{},
    m_receiveBufferStage2{},
    m_sendBufferStage1{},
    m_sendBufferStage2{}
{
    setRemoteEndpoint();
}

void Connection::start()
{
    LOG_DEBUG("{}: started connection", m_remoteEndpoint);
    startReceive();
}

void Connection::stop()
{
    if(!m_socket.is_open()) {
        return;
    }

    LOG_DEBUG("{}: stopped connection", m_remoteEndpoint);

    asio::error_code ec;
    m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
    if(ec) {
        LOG_WARN("{}: failed to shutdown socket, {} ({})", m_remoteEndpoint,
                 ec.message(), ec.value());
    }

    m_socket.close();
    if(ec) {
        LOG_WARN("{}: failed to close socket, {} ({})", m_remoteEndpoint,
                 ec.message(), ec.value());
    }

    m_connectionManager.remove(*this);
}

void Connection::setRemoteEndpoint()
{
    // The remote endpoint is logged when an event occurs in the connection to
    // help track what a connection is doing.
    //
    // When called on a socket that's in an error state, `remote_endpoint()`
    // returns an endpoint that could differ from when the socket isn't in an
    // error state. This hides which connection object an log entry comes from,
    // defeating the purpose of logging the endpoint. To resolve this, the
    // result of `remote_endpoint()` is cached for uses throughout the
    // connection's lifetime.
    //
    // One "issue" (not really an issue, just something that could be
    // misleading when debugging) this approach brings is that the cached
    // endpoint could point to a completely different socket.
    //
    // Another approach would be to assign a UUID to each connection, which
    // would be logged instead of the endpoint. Although, it's still imprortant
    // to know the endpoint, but this could be logged once at the start,
    // essentially mapping the endpoint to the UUID. However, there is no
    // current need for this approach right now, so the previous approach is
    // sufficient.
    asio::error_code ec;
    m_remoteEndpoint = m_socket.remote_endpoint(ec);
    if(ec) {
        LOG_WARN("{}: failed to get remote endpoint, {} ({})", m_remoteEndpoint,
                 ec.message(), ec.value());
    }
}

void Connection::startReceive()
{
    LOG_DEBUG("{}: started receive", m_remoteEndpoint);
    m_socket.async_receive(asio::buffer(m_receiveBufferStage1),
                           [self = shared_from_this()](
                               asio::error_code ec, std::size_t bytesReceived) {
                               self->receiveToken(ec, bytesReceived);
                           });
}

void Connection::receiveToken(asio::error_code ec, std::size_t bytesReceived)
{
    if(ec) {
        LOG_WARN("{}: failed to receive, {} ({})", m_remoteEndpoint,
                 ec.message(), ec.value());
        stop();
        return;
    }

    LOG_DEBUG("{}: received {} bytes", m_remoteEndpoint, bytesReceived);
    if(transferReceiveBuffers(bytesReceived)) {
        m_threadPool.queue(
            [self = shared_from_this()]() { self->handleReceivedDataLoop(); });
    }

    startReceive();
}

void Connection::handleReceivedDataLoop()
{
    while(true) {
        auto data = extractReceiveBufferStage2();
        if(data.empty()) {
            break;
        }

        handleReceivedData(std::move(data));
    }
}

void Connection::handleReceivedData(common::Buffer data)
{
    // TODO: This handling logic is only for testing purposes. It needs to be
    // forwarded to some message parser for the application logic.
    std::ostringstream stream;
    common::utility::hexdump(stream,
                             common::BufferView{data.data(), data.size()});
    LOG_DEBUG("{}", stream.str());

    const std::string response = "hello client, this is the server";
    send(common::BufferView{reinterpret_cast<const std::byte*>(response.data()),
                            response.size()});
}

void Connection::send(common::BufferView data)
{
    if(insertSendBufferStage1(data)) {
        startSend();
    }
}

void Connection::startSend()
{
    transferSendBuffers();
    if(m_sendBufferStage2.empty()) {
        return;
    }

    LOG_DEBUG("{}: started send", m_remoteEndpoint);
    m_socket.async_send(asio::buffer(m_sendBufferStage2),
                        [self = shared_from_this()](asio::error_code ec,
                                                    std::size_t bytesSent) {
                            self->sendToken(ec, bytesSent);
                        });
}

void Connection::sendToken(asio::error_code ec, std::size_t bytesSent)
{
    if(ec) {
        LOG_WARN("{}: failed to send, {} ({})", m_remoteEndpoint, ec.message(),
                 ec.value());
        stop();
        return;
    }

    LOG_DEBUG("{}: sent {} bytes", m_remoteEndpoint, bytesSent);
    auto endIt =
        m_sendBufferStage2.begin() + common::utility::makeSigned(bytesSent);
    m_sendBufferStage2.erase(m_sendBufferStage2.begin(), endIt);
    startSend();
}

bool Connection::transferReceiveBuffers(std::size_t bytesReceived)
{
    const common::BufferView received{m_receiveBufferStage1.data(),
                                      bytesReceived};
    auto buffer = m_receiveBufferStage2.lock();
    const bool wasEmpty = buffer->empty();
    buffer->insert(buffer->begin(), received.begin(), received.end());
    return wasEmpty;
}

common::Buffer Connection::extractReceiveBufferStage2()
{
    common::Buffer result;

    auto buffer = m_receiveBufferStage2.lock();
    if(!buffer->empty()) {
        result = std::move(*buffer);
        buffer->clear();
    }

    return result;
}

bool Connection::insertSendBufferStage1(common::BufferView data)
{
    auto buffer = m_sendBufferStage1.lock();
    const bool wasEmpty = buffer->empty();
    buffer->insert(buffer->begin(), data.begin(), data.end());
    return wasEmpty;
}

void Connection::transferSendBuffers()
{
    auto buffer = m_sendBufferStage1.lock();
    if(!buffer->empty()) {
        m_sendBufferStage2.insert(m_sendBufferStage2.begin(), buffer->begin(),
                                  buffer->end());
        buffer->clear();
    }
}
}
