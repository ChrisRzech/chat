#pragma once

#include "chat/common/InputByteStream.hpp"
#include "chat/common/OutputByteStream.hpp"

#include <cstdint>

namespace chat::messages
{
/**
 * @brief A message sent from a client to a server.
 *
 * @details The @c Request class is a message that is sent from a client to a
 * server. It indicates that the client wants the server to perform an
 * operation.
 *
 * A @c Request must be serialized into bytes before being sent. When received,
 * the receiver should deserialize the bytes back into a @c Request. Using
 * @c deserialize() on its own has some complications as the correct derived
 * message type must be created before @c deserialize() can be used. Instead,
 * @c chat::messages::deserializeRequest() should be used to deserialize the
 * bytes and create a @c Request.
 *
 * @c Request is an abstract base class for other classes to derive from.
 * A class that derives from @c Request corresponds to a specific operation.
 * When a new derived class is created, a unique value should be added to
 * @c Type.
 */
class Request
{
public:
    /**
     * @brief The type of a request.
     *
     * @details Each value corresponds to a derived type of @c Request.
     */
    enum class Type : std::uint8_t
    {
        Ping
    };

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Request(const Request& other) = delete;
    Request& operator=(const Request& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Request(Request&& other) = delete;
    Request& operator=(Request&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the @c Request.
     */
    virtual ~Request() = default;

    /**
     * @brief Get the type of the request.
     *
     * @return The type of the request.
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
     * @brief Construct a @c Request.
     *
     * @param type The type of the request.
     */
    explicit Request(Type type);

private:
    Type m_type;
};
}
