#pragma once

#include "chat/common/InputByteStream.hpp"
#include "chat/common/OutputByteStream.hpp"

#include <cstdint>

namespace chat::messages
{
/**
 * @brief A message sent from a server to a client.
 *
 * @details The @c Response class is a message that is sent from a server to a
 * client. It is in response to a @c Request that a client has sent to a server.
 *
 * A @c Response must be serialized into bytes before being sent. When received,
 * the receiver should deserialize the bytes back into a @c Response. Using
 * @c deserialize() on its own has some complications as the correct derived
 * message type must be created before @c deserialize() can be used. Instead,
 * @c chat::messages::deserializeResponse() should be used to deserialize the
 * bytes and create a @c Response.
 *
 * @c Response is an abstract base class for other classes to derive from.
 * A class that derives from @c Response corresponds to a response to a specific
 * request. When a new derived class is created, a unique value should be added
 * to @c Type.
 */
class Response
{
public:
    /**
     * @brief The type of a response.
     *
     * @details Each value corresponds to a derived type of @c Response.
     */
    enum class Type : std::uint8_t
    {
        Pong
    };

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Response(const Response& other) = delete;
    Response& operator=(const Response& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Response(Response&& other) = delete;
    Response& operator=(Response&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the @c Response.
     */
    virtual ~Response() = default;

    /**
     * @brief Get the type of the response.
     *
     * @return The type of the response.
     */
    [[nodiscard]] Type getType() const;

    /**
     * @brief Serialize into a stream.
     *
     * @param stream The stream to serialize into.
     */
    virtual void serialize(common::OutputByteStream& stream) const;

    /**
     * @brief Deserialize from a stream.
     *
     * @param stream The stream to deserialize from.
     *
     * @return True if successfully deserialized; otherwise, false.
     */
    [[nodiscard]] virtual bool deserialize(common::InputByteStream& stream) = 0;

protected:
    /**
     * @brief Construct a @c Response.
     *
     * @param type The type of the response.
     */
    explicit Response(Type type);

private:
    Type m_type;
};
}
