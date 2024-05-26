#pragma once

#include "chat/common/InputByteStream.hpp"
#include "chat/common/OutputByteStream.hpp"

#include <cstdint>

namespace chat::messages
{

/**
 * @brief A serializable object to allow data communication between
 * applications.
 *
 * @details The purpose of a message is to simplify transfering data between
 * applications in a structured format that is known to the applications. The
 * applications determine the meaning of each message in their own context.
 * Messages are designed to be serializable into a buffer which can then be
 * transported to another application using a transportation interface (e.g.
 * socket).
 *
 * A message can be serialized into a buffer using @c serialize(). The buffer
 * can then be deserialized using @c deserialize() on the apporpriate message
 * type. Using @c deserialize() on its own has some complications as the correct
 * derived message type must be created before deserializing the buffer along
 * with making sure the buffer is clear of any metadata. Therefore, it is
 * recommended to use the @c chat::messages::serialize() and
 * @c chat::messages::deserialize() utility functions to perform all message
 * serialization and deserialization.
 *
 * Messages are structured in a hierarchy of types. At the top level, there is
 * @c Message which encapsulates all messages. The next level splits into three
 * major message types: @c Close, , @c Request, and @c Response. The next level,
 * usually the last, is messages containing message-specific data that
 * applications will use.
 */
class Message
{
public:
    /**
     * @brief The type of a message.
     */
    enum class Type : std::uint32_t
    {
        Close,
        Request,
        Response
    };

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Message(const Message& other) = delete;
    Message& operator=(const Message& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Message(Message&& other) = delete;
    Message& operator=(Message&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the message.
     */
    virtual ~Message() = default;

    /**
     * @brief Get the message type.
     *
     * @return The message type.
     */
    [[nodiscard]] Type getType() const;

    /**
     * @brief Serialize the message into a stream.
     *
     * @param stream The stream to serialize the message into.
     */
    virtual void serialize(common::OutputByteStream& stream) const;

    /**
     * @brief Deserialize the message from a stream.
     *
     * @param stream The stream to deserialize the message from.
     *
     * @return True if the message successfully deserialized; otherwise, false.
     */
    [[nodiscard]] virtual bool deserialize(common::InputByteStream& stream) = 0;

protected:
    /**
     * @brief Construct a message.
     *
     * @param type The type of the message.
     */
    explicit Message(Type type);

private:
    Type m_type;
};

}
