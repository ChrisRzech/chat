#pragma once

#include "chat/common/InputByteStream.hpp"
#include "chat/common/OutputByteStream.hpp"

#include "chat/messages/Message.hpp"

#include <cstdint>

namespace chat::messages
{

/**
 * @brief A request message.
 *
 * @details A request is sent from the client to the server when the client
 * wants the server to perform an operation. The client should expect a
 * @c Response from the server for each request sent.
 */
class Request : public Message
{
public:
    /**
     * @brief The type of a request.
     */
    enum class Type : std::uint32_t
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
     * @brief Destroy the request.
     */
    ~Request() override = default;

    /**
     * @brief Get the type of the request.
     *
     * @return The type of the request.
     */
    [[nodiscard]] Type getType() const;

    /**
     * @brief Serialize the message into a stream.
     *
     * @param stream The stream to serialize the message into.
     */
    void serialize(common::OutputByteStream& stream) const override;

protected:
    /**
     * @brief Construct a request.
     *
     * @param type The type of the request.
     */
    explicit Request(Type type);

private:
    Type m_type;
};

}
