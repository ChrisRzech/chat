#include "chat/messages/Serializer.hpp"

#include "chat/messages/Close.hpp"
#include "chat/messages/Message.hpp"
#include "chat/messages/Ping.hpp"
#include "chat/messages/Pong.hpp"
#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include <stdexcept>

namespace chat::messages
{

Serializer::Serializer()
  : m_requestFactory{},
    m_responseFactory{}
{
    //Requests
    m_requestFactory.registerType<Ping>(Request::Type::Ping);

    //Responses
    m_responseFactory.registerType<Pong>(Response::Type::Pong);
}

void Serializer::serialize(const Message& message, sf::Packet& packet) const
{
    message.toPacket(packet);
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
        message = createMessage(m_requestFactory, packet);
        break;

    case Message::Type::Response:
        message = createMessage(m_responseFactory, packet);
        break;
    }

    if(!message.has_value() || !message.value()->fromPacket(packet))
    {
        message.value()->fromPacket(packet);
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
std::unique_ptr<BaseType> Serializer::Factory<BaseType, KeyType>::createType(KeyType key) const
{
    auto iter = m_registry.find(key);
    if(iter == m_registry.end())
    {
        throw std::invalid_argument{"Key is not registered"};
    }

    /*
    The constructed object is of type `Serializable`. However, a type derived from `Serializable` needs to be returned. Therefore,
    the ownership of the object must be transfered to a new smart pointer which holds the derived type.
    */
    return iter->second();
}

template<typename BaseType, typename MessageType>
std::optional<std::unique_ptr<Message>> Serializer::createMessage(const Factory<BaseType, MessageType>& factory, sf::Packet& packet) const
{
    std::underlying_type_t<MessageType> typeValue;
    if(!(packet >> typeValue))
    {
        return std::nullopt;
    }
    auto type = static_cast<MessageType>(typeValue);

    if(!factory.isTypeRegistered(type))
    {
        return std::nullopt;
    }
    return factory.createType(type);
}

}
