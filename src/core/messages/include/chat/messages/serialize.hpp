#pragma once

#include "chat/common/ByteSpan.hpp"
#include "chat/common/ByteString.hpp"

#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include <memory>
#include <optional>

namespace chat::messages
{
/**
 * @brief Serialize a @c Request into a byte string.
 *
 * @param request The @c Request to serialize.
 *
 * @return A byte string containing the serialized @c Request.
 */
[[nodiscard]] common::ByteString serialize(const Request& request);

/**
 * @brief Serialize a @c Response into a byte string.
 *
 * @param response The @c Response to serialize.
 *
 * @return A byte string containing the serialized @c Response.
 */
[[nodiscard]] common::ByteString serialize(const Response& response);

/**
 * @brief Create a @c Request from a byte string containing a serialized
 * @c Request.
 *
 * @details There may be data left over in the byte string since only enough
 * data to create the @c Request is extracted from the byte string.
 *
 * @param bytes The byte string containing a serialized @c Request.
 *
 * @return A deserialized @c Request from the byte string. No value if the
 * process failed.
 */
[[nodiscard]] std::optional<std::unique_ptr<Request>> deserializeRequest(
    const common::ByteSpan& bytes);

/**
 * @brief Create a @c Response from a byte string containing a serialized
 * @c Response.
 *
 * @details There may be data left over in the byte string since only enough
 * data to create the @c Response is extracted from the byte string.
 *
 * @param bytes The byte string containing a serialized @c Response.
 *
 * @return A deserialized @c Response from the byte string. No value if the
 * process failed.
 */
[[nodiscard]] std::optional<std::unique_ptr<Response>> deserializeResponse(
    const common::ByteSpan& bytes);
}
