#pragma once

#include "chat/common/InputByteStream.hpp"
#include "chat/common/OutputByteStream.hpp"

#include "chat/messages/Response.hpp"

namespace chat::messages
{

/**
 * @brief A pong response.
 */
class Pong : public Response
{
public:
    /**
     * @brief Construct a pong response.
     */
    Pong();

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
