#pragma once

#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include <SFML/Network/Packet.hpp>

#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>

namespace chat::messages
{

class Message;

/*
TODO Feels like this class should be some kind of static object, or the functions are static. The real problem is the types needs to be
registered each time it is constructed. While not being a major factor in performance, registering types only needs to be done once.
*/
class Serializer
{
public:
    Serializer();

    Serializer(const Serializer& other) = delete;

    Serializer& operator=(const Serializer& other) = delete;

    Serializer(Serializer&& other) = delete;

    Serializer& operator=(Serializer&& other) = delete;

    ~Serializer() = default;

    void serialize(const Message& message, sf::Packet& packet) const;

    [[nodiscard]] std::optional<std::unique_ptr<Message>> deserialize(sf::Packet& packet) const;

private:
    template<typename BaseType, typename KeyType>
    class Factory
    {
    public:
        Factory() = default;

        Factory(const Factory& other) = delete;

        Factory& operator=(const Factory& other) = delete;

        Factory(Factory&& other) = delete;

        Factory& operator=(Factory&& other) = delete;

        ~Factory() = default;

        template<typename DerivedType>
        void registerType(KeyType key);

        [[nodiscard]] bool isTypeRegistered(KeyType key) const;

        [[nodiscard]] std::unique_ptr<BaseType> createType(KeyType key) const;

    private:
        std::unordered_map<KeyType, std::function<std::unique_ptr<BaseType>()>> m_registry;
    };

    template<typename BaseType, typename MessageType>
    [[nodiscard]] std::optional<std::unique_ptr<Message>> createMessage(const Factory<BaseType, MessageType>& factory,
        sf::Packet& packet) const;

    Factory<Request, Request::Type> m_requestFactory;
    Factory<Response, Response::Type> m_responseFactory;
};

}
