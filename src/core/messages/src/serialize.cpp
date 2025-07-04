#include "chat/messages/serialize.hpp"

#include "chat/common/Buffer.hpp"
#include "chat/common/BufferView.hpp"
#include "chat/common/InputByteStream.hpp"
#include "chat/common/OutputByteStream.hpp"
#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"
#include "chat/messages/request/Ping.hpp"
#include "chat/messages/response/Pong.hpp"

#include <memory>
#include <optional>
#include <type_traits>

namespace chat::messages
{
namespace
{
std::unique_ptr<Request> createRequest(Request::Type type)
{
    std::unique_ptr<Request> request;
    switch(type) {
    case Request::Type::Ping:
        request = std::make_unique<Ping>();
        break;
    }
    return request;
}

std::unique_ptr<Response> createResponse(Response::Type type)
{
    std::unique_ptr<Response> response;
    switch(type) {
    case Response::Type::Pong:
        response = std::make_unique<Pong>();
        break;
    }
    return response;
}

template<typename Message>
common::Buffer serializeMessage(const Message& message)
{
    common::OutputByteStream innerStream;
    message.serialize(innerStream);
    const auto inner = innerStream.getData();

    common::OutputByteStream outerStream;
    outerStream << inner;
    const auto outer = outerStream.getData();
    return outer;
}

template<typename Message, typename Factory>
std::optional<std::unique_ptr<Message>> deserializeMessage(
    const common::BufferView& bytes, const Factory& factory)
{
    common::InputByteStream outerStream{bytes};

    common::Buffer inner;
    if(!(outerStream >> inner)) {
        return {};
    }

    const common::BufferView innerView{inner.data(), inner.size()};
    common::InputByteStream innerStream{innerView};

    if(inner.size() != innerStream.getReadableCount()) {
        return {};
    }

    std::underlying_type_t<typename Message::Type> typeValue{};
    if(!(innerStream >> typeValue)) {
        return {};
    }

    auto message = factory(static_cast<typename Message::Type>(typeValue));
    if(message == nullptr) {
        return {};
    }

    if(!message->deserialize(innerStream)) {
        return {};
    }

    return message;
}
}

common::Buffer serialize(const Request& request)
{
    return serializeMessage(request);
}

common::Buffer serialize(const Response& response)
{
    return serializeMessage(response);
}

std::optional<std::unique_ptr<Request>> deserializeRequest(
    const common::BufferView& bytes)
{
    return deserializeMessage<Request>(bytes, createRequest);
}

std::optional<std::unique_ptr<Response>> deserializeResponse(
    const common::BufferView& bytes)
{
    return deserializeMessage<Response>(bytes, createResponse);
}
}
