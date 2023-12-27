#include "chat/messages/Serializer.hpp"

#include "chat/messages/Close.hpp"
#include "chat/messages/Message.hpp"
#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include "chat/messages/request/Ping.hpp"

#include "chat/messages/response/Pong.hpp"

#include <stdexcept>
#include <mutex>

namespace chat::messages
{

Serializer::Serializer()
{
    static std::once_flag flag;
    std::call_once(flag, &Serializer::registerMessages);
}

sf::Packet Serializer::serialize(const Message& message) const
{
    sf::Packet packet;
    message.serialize(packet);
    return packet;
}

std::optional<std::unique_ptr<Message>> Serializer::deserialize(sf::Packet& packet) const
{
    std::underlying_type_t<Message::Type> messageTypeValue;
    if(!(packet >> messageTypeValue))
    {
        return std::nullopt;
    }

    std::optional<std::unique_ptr<Message>> message;
    switch(static_cast<Message::Type>(messageTypeValue))
    {
    case Message::Type::Close:
        message = std::make_optional(std::make_unique<Close>());
        break;

    case Message::Type::Request:
        message = createMessage(s_requestFactory, packet);
        break;

    case Message::Type::Response:
        message = createMessage(s_responseFactory, packet);
        break;
    }

    if(!message.has_value() || !message.value()->deserialize(packet))
    {
        return std::nullopt;
    }

    return message;
}

template<typename BaseType, typename KeyType>
template<typename DerivedType>
void Serializer::Factory<BaseType, KeyType>::registerType(KeyType key)
{
    static_assert(std::is_base_of_v<BaseType, DerivedType>, "BaseType is not a base of DerivedType");

    auto [iter, success] = m_registry.emplace(key, []{return std::make_unique<DerivedType>();});
    if(!success)
    {
        throw std::invalid_argument{"Key is already registered"};
    }
}

template<typename BaseType, typename KeyType>
bool Serializer::Factory<BaseType, KeyType>::isTypeRegistered(KeyType key) const
{
    return m_registry.count(key) == 1;
}

template<typename BaseType, typename KeyType>
std::unique_ptr<BaseType> Serializer::Factory<BaseType, KeyType>::createObject(KeyType key) const
{
    auto iter = m_registry.find(key);
    if(iter == m_registry.end())
    {
        throw std::invalid_argument{"Key is not registered"};
    }

    return iter->second();
}

void Serializer::registerMessages()
{
    //Requests
    s_requestFactory.registerType<Ping>(Request::Type::Ping);

    //Responses
    s_responseFactory.registerType<Pong>(Response::Type::Pong);
}

template<typename BaseType, typename KeyType>
std::optional<std::unique_ptr<Message>> Serializer::createMessage(const Factory<BaseType, KeyType>& factory, sf::Packet& packet) const
{
    std::underlying_type_t<KeyType> typeValue;
    if(!(packet >> typeValue))
    {
        return std::nullopt;
    }
    auto type = static_cast<KeyType>(typeValue);

    if(!factory.isTypeRegistered(type))
    {
        return std::nullopt;
    }
    return factory.createObject(type);
}

}
