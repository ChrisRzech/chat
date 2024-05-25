#pragma once

#include "chat/common/ByteSpan.hpp"
#include "chat/common/ByteString.hpp"

#include "chat/messages/Message.hpp"

#include <memory>
#include <optional>

namespace chat::messages
{

/**
 * @brief Serialize a message into a byte string.
 *
 * @param message The message to serialize.
 *
 * @return A byte string containing the serialized message.
 */
[[nodiscard]] common::ByteString serialize(const Message& message);

/**
 * @brief Create a message from a byte string containing a serialized message.
 *
 * @details There may be data left over in the byte string since only enough
 * data to create the message is extracted from the byte string.
 *
 * @param bytes The byte string containing a serialized message.
 *
 * @return A deserialized message from the byte string. No value if the process
 * failed.
 */
[[nodiscard]] std::optional<std::unique_ptr<Message>> deserialize(
    const common::ByteSpan& bytes);

}
