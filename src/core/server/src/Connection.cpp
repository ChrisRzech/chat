#include "Connection.hpp"

#include "ConnectionManager.hpp"

#include "chat/common/Logging.hpp"

#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"
#include "chat/messages/serialize.hpp"

// TODO: Remove includes
#include <iostream>
#include "chat/common/utility.hpp"

namespace chat::server
{
Connection::Connection(asio::ip::tcp::socket&& socket,
                       ConnectionManager& connectionManager,
                       common::ThreadPool& threadPool)
  : m_socket{std::move(socket)},
    m_connectionManager{connectionManager},
    m_threadPool{threadPool},
    m_receiving{},
    m_sending{}
{}

void Connection::start()
{
    LOG_DEBUG << m_socket.remote_endpoint() << ": started connection";
    startReceive();
}

void Connection::stop()
{
    LOG_DEBUG << m_socket.remote_endpoint() << ": stopped connection";
    m_socket.close();
    m_connectionManager.remove(*this);
}

void Connection::startReceive()
{
    LOG_DEBUG << m_socket.remote_endpoint() << ": started receive";
    m_socket.async_receive(
        asio::buffer(m_receiving),
        [this](asio::error_code ec, std::size_t bytesReceived) {
            receiveToken(ec, bytesReceived);
        });
}

void Connection::receiveToken(asio::error_code ec, std::size_t bytesReceived)
{
    if(ec) {
        LOG_ERROR << m_socket.remote_endpoint() << ": failed to receive, "
                  << ec.message() << " (" << ec.value() << ")";
        stop();
        return;
    }

    LOG_DEBUG << m_socket.remote_endpoint() << ": received " << bytesReceived
              << " bytes";

    common::ByteString receivedData{
        m_receiving.begin(), std::next(m_receiving.begin(), bytesReceived)};
    m_threadPool.queue([this, data = std::move(receivedData)]() {
        handleReceiveJob(std::move(data));
    });

    startReceive();
}

void Connection::startSend()
{
    // No reason to start sending if there is nothing to send
    if(m_sending.empty()) {
        return;
    }

    LOG_DEBUG << m_socket.remote_endpoint() << ": started send";
    m_socket.async_send(asio::buffer(m_sending),
                        [this](asio::error_code ec, std::size_t bytesSent) {
                            sendToken(ec, bytesSent);
                        });
}

void Connection::sendToken(asio::error_code ec, std::size_t bytesSent)
{
    if(ec) {
        LOG_ERROR << m_socket.remote_endpoint()
                  << ": failed to send: " << ec.message() << " (" << ec.value()
                  << ")";
        stop();
        return;
    }

    LOG_DEBUG << m_socket.remote_endpoint() << ": sent " << bytesSent
              << " bytes";

    dequeueSendingBuffer(bytesSent);

    startSend();
}

void Connection::handleReceiveJob(common::ByteString data)
{
    common::utility::hexdump(std::cout,
                             common::ByteSpan{data.data(), data.size()});

    // TODO: Whenever `m_sending` is modified, it needs to be done in a
    // thread-safe manner because the I/O thread will attempt to modify it as
    // well as this thread.
    const std::string response = "hello client";
    enqueueSendingBuffer(common::ByteSpan{
        reinterpret_cast<const std::byte*>(response.data()), response.size()});

    // TODO: There could be a potential async send operation in process when
    // this is called. Would this cause issues? Should there be checks to
    // prevent calling it twice if the connection has an outstanding async
    // send operation?
    //
    // https://cplusplus.com/forum/general/213452/:
    // > The program must ensure that the stream performs no other write
    // > operations (such as async_write, the stream's async_write_some
    // > function, or any other composed operations that perform writes)
    // until > this operation completes. Probably be on the safe side and
    // assume that you shouldn't do this.
    startSend();
}

void Connection::enqueueSendingBuffer(common::ByteSpan data)
{
    m_sending.insert(m_sending.end(), data.begin(), data.end());
}

void Connection::dequeueSendingBuffer(std::size_t bytesSent)
{
    m_sending.erase(m_sending.begin(), std::next(m_sending.begin(), bytesSent));
}
}
