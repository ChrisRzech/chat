#pragma once

#include "chat/common/InputByteStream.hpp"
#include "chat/common/OutputByteStream.hpp"

#include "chat/messages/Message.hpp"

namespace chat::messages
{

/**
 * @brief A close message.
 *
 * @details This message is sent from the client to the server to indicate that
 * the client wants to close the connection to the server. Once the server
 * receives this message, it can assume that the client will not send anymore
 * messages and the connection can be closed.
 */
class Close : public Message
{
public:
    /**
     * @brief Construct a close message.
     */
    Close();

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Close(const Close& other) = delete;
    Close& operator=(const Close& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Close(Close&& other) = delete;
    Close& operator=(Close&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the close message.
     */
    ~Close() override = default;

    /**
     * @brief Serialize the message into a stream.
     *
     * @param stream The stream to serialize the message into.
     */
    void serialize(common::OutputByteStream& stream) const override;

    /**
     * @brief Deserialize the message from a stream.
     *
     * @param stream The stream to deserialize the message from.
     *
     * @return True if the message successfully deserialized; otherwise, false.
     */
    [[nodiscard]] bool deserialize(common::InputByteStream& stream) override;
};

}
