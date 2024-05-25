#include "chat/messages/serialize.hpp"

#include "chat/common/InputByteStream.hpp"
#include "chat/common/OutputByteStream.hpp"

#include "chat/messages/Close.hpp"
#include "chat/messages/Message.hpp"
#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include "chat/messages/request/Ping.hpp"

#include "chat/messages/response/Pong.hpp"

#include <functional>
#include <mutex>
#include <unordered_map>

namespace chat::messages
{

namespace
{

template<typename KeyType, typename BaseType>
using Factory =
    std::unordered_map<KeyType, std::function<std::unique_ptr<BaseType>()>>;
using RequestFactory = Factory<Request::Type, Request>;
using ResponseFactory = Factory<Response::Type, Response>;

template<typename T>
std::unique_ptr<T> create()
{
    return std::make_unique<T>();
}

const RequestFactory& getRequestFactory()
{
    // Using `thread_local` here instead of just `static` removes the
    // requirement of needing to use synchronization mechanisms
    thread_local const RequestFactory factory = {
        {Request::Type::Ping, create<Ping>}};
    return factory;
}

const ResponseFactory& getResponseFactory()
{
    // Using `thread_local` here instead of just `static` removes the
    // requirement of needing to use synchronization mechanisms
    thread_local const ResponseFactory factory = {
        {Response::Type::Pong, create<Pong>}};
    return factory;
}

template<typename KeyType, typename BaseType>
std::optional<std::unique_ptr<Message>> createMessage(
    const Factory<KeyType, BaseType>& factory, common::InputByteStream& stream)
{
    std::underlying_type_t<KeyType> typeValue;
    if(!(stream >> typeValue)) {
        return std::nullopt;
    }
    auto type = static_cast<KeyType>(typeValue);

    if(factory.count(type) != 1) {
        return std::nullopt;
    }

    return std::make_optional(factory.at(type)());
}

}

common::ByteString serialize(const Message& message)
{
    common::OutputByteStream stream;
    message.serialize(stream);
    return stream.getData();
}

std::optional<std::unique_ptr<Message>> deserialize(
    const common::ByteSpan& bytes)
{
    common::InputByteStream stream{bytes};

    std::underlying_type_t<Message::Type> messageTypeValue{};
    if(!(stream >> messageTypeValue)) {
        return std::nullopt;
    }

    std::optional<std::unique_ptr<Message>> message;
    switch(static_cast<Message::Type>(messageTypeValue)) {
    case Message::Type::Close:
        message = std::make_optional(std::make_unique<Close>());
        break;

    case Message::Type::Request:
        message = createMessage(getRequestFactory(), stream);
        break;

    case Message::Type::Response:
        message = createMessage(getResponseFactory(), stream);
        break;
    }

    if(!message.has_value() || !message.value()->deserialize(stream)) {
        return std::nullopt;
    }

    return message;
}

}
