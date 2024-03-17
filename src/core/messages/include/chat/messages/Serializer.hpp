#pragma once

#include "chat/messages/Message.hpp"
#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include <SFML/Network/Packet.hpp>

#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>

namespace chat::messages
{

/**
 * @brief A tool that simplifies the process of serializing and deserializing messages.
 */
class Serializer
{
public:
    /**
     * @brief Construct a serializer.
     */
    Serializer();

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Serializer(const Serializer& other) = delete;
    Serializer& operator=(const Serializer& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Serializer(Serializer&& other) = delete;
    Serializer& operator=(Serializer&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the serializer.
     */
    ~Serializer() = default;

    /**
     * @brief Serialize a message into a packet.
     *
     * @param message The message to serialize.
     *
     * @return A packet containing the serialized message.
     */
    [[nodiscard]] sf::Packet serialize(const Message& message) const;

    /**
     * @brief Create a message from a packet containing a serialized message.
     *
     * @details There may be data left over in the packet since only enough data to create the message is extracted from the packet.
     *
     * @param packet The packet containing a serialized message.
     *
     * @return A deserialized message from the packet. No value if the process failed.
     */
    [[nodiscard]] std::optional<std::unique_ptr<Message>> deserialize(sf::Packet& packet) const;

private:
    /**
     * @brief A factory for creating objects of types in a class inheritance hierarchy.
     *
     * @details Types must be registered to the factory before attempting to create objects of that type through the factory. Keys and types
     * have a many-to-one relationship, meaning that a key cannot be registered to different types.
     *
     * @tparam BaseType The base type of the class inheritance hierarchy.
     *
     * @tparam KeyType The type for keys.
     */
    template<typename BaseType, typename KeyType>
    class Factory
    {
    public:
        /**
         * @brief Construct a factory.
         */
        Factory() = default;

        /**
         * @brief Copy operations are disabled.
         * @{
         */
        Factory(const Factory& other) = delete;
        Factory& operator=(const Factory& other) = delete;
        /** @} */

        /**
         * @brief Move operations are disabled.
         * @{
         */
        Factory(Factory&& other) = delete;
        Factory& operator=(Factory&& other) = delete;
        /** @} */

        /**
         * @brief Destroy the factory.
         */
        ~Factory() = default;

        /**
         * @brief Register a type.
         *
         * @tparam DerivedType The type to register.
         *
         * @param key The key to associate the type with.
         *
         * @throws An @c std::invalid_argument if the key is already registered.
         */
        template<typename DerivedType>
        void registerType(KeyType key);

        /**
         * @brief Check if the key is registered.
         *
         * @param key The key to check for.
         *
         * @return True if the key is registered; otherwise, false.
         */
        [[nodiscard]] bool isTypeRegistered(KeyType key) const;

        /**
         * @brief Create an object with the type associated with a key.
         *
         * @param key The key associated to a registered type.
         *
         * @return An object with the type associated to the key.
         *
         * @throws An @c std::invalid_argument if the key is not registered.
         */
        [[nodiscard]] std::unique_ptr<BaseType> createObject(KeyType key) const;

    private:
        std::unordered_map<KeyType, std::function<std::unique_ptr<BaseType>()>> m_registry;
    };

    /**
     * @brief Register messages to their appropriate factories.
     */
    static void registerMessages();

    /**
     * @brief Create a message.
     *
     * @details The message type is extracted from the packet and provided to the factory for it to make a registered type. The message type
     * is used as the key for the type for the factory to create.
     *
     * @tparam BaseType The base type for the factory.
     *
     * @tparam KeyType The key type for the factory.
     *
     * @param factory The factory to use for createing the message.
     *
     * @param packet The packet to extract information from.
     *
     * @return A message created by the factory. No value if the message type could not be extracted from the packet or the type is not
     * registered in the factory.
     */
    template<typename BaseType, typename KeyType>
    [[nodiscard]] std::optional<std::unique_ptr<Message>> createMessage(const Factory<BaseType, KeyType>& factory,
        sf::Packet& packet) const;

    inline static Factory<Request, Request::Type> s_requestFactory;
    inline static Factory<Response, Response::Type> s_responseFactory;
};

}
