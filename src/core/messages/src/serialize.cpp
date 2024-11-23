#include "chat/messages/serialize.hpp"

#include "chat/common/InputByteStream.hpp"
#include "chat/common/OutputByteStream.hpp"

#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include "chat/messages/request/Ping.hpp"

#include "chat/messages/response/Pong.hpp"

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

template<typename Message, typename Factory>
std::optional<std::unique_ptr<Message>> deserializeMessage(
    const common::ByteSpan& bytes, const Factory& factory)
{
    common::InputByteStream stream{bytes};

    std::underlying_type_t<typename Message::Type> typeValue{};
    if(!(stream >> typeValue)) {
        return {};
    }

    auto message = factory(static_cast<typename Message::Type>(typeValue));
    if(message == nullptr) {
        return {};
    }

    if(!message->deserialize(stream)) {
        return {};
    }

    return message;
}
}

common::ByteString serialize(const Request& request)
{
    common::OutputByteStream stream;
    request.serialize(stream);
    return stream.getData();
}

common::ByteString serialize(const Response& response)
{
    common::OutputByteStream stream;
    response.serialize(stream);
    return stream.getData();
}

std::optional<std::unique_ptr<Request>> deserializeRequest(
    const common::ByteSpan& bytes)
{
    return deserializeMessage<Request>(bytes, createRequest);
}

std::optional<std::unique_ptr<Response>> deserializeResponse(
    const common::ByteSpan& bytes)
{
    return deserializeMessage<Response>(bytes, createResponse);
}
}
