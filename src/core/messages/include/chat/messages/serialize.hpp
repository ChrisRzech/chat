#pragma once

#include "chat/common/Buffer.hpp"
#include "chat/common/BufferView.hpp"
#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include <memory>
#include <optional>

namespace chat::messages
{
/**
 * @brief Serialize a @c Request into a buffer.
 *
 * @param request The @c Request to serialize.
 *
 * @return A buffer containing the serialized @c Request.
 */
[[nodiscard]] common::Buffer serialize(const Request& request);

/**
 * @brief Serialize a @c Response into a buffer.
 *
 * @param response The @c Response to serialize.
 *
 * @return A buffer containing the serialized @c Response.
 */
[[nodiscard]] common::Buffer serialize(const Response& response);

/**
 * @brief Create a @c Request from a buffer containing a serialized
 * @c Request.
 *
 * @details There may be data left over in the buffer since only enough
 * data to create the @c Request is extracted from the buffer.
 *
 * @param bytes The buffer containing a serialized @c Request.
 *
 * @return A deserialized @c Request from the buffer. No value if the
 * process failed.
 */
[[nodiscard]] std::optional<std::unique_ptr<Request>> deserializeRequest(
    const common::BufferView& bytes);

/**
 * @brief Create a @c Response from a buffer containing a serialized
 * @c Response.
 *
 * @details There may be data left over in the buffer since only enough
 * data to create the @c Response is extracted from the buffer.
 *
 * @param bytes The buffer containing a serialized @c Response.
 *
 * @return A deserialized @c Response from the buffer. No value if the process
 * failed.
 */
[[nodiscard]] std::optional<std::unique_ptr<Response>> deserializeResponse(
    const common::BufferView& bytes);
}
