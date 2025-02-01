#pragma once

#include "chat/common/ByteSpan.hpp"
#include "chat/common/Result.hpp"

#include "chat/messages/Request.hpp"

#include <memory>

namespace chat::messages
{
class IncrementalRequestDeserializer
{
public:
    enum class FailureReason
    {
        Partial,
        Error,
    };

    common::Result<std::unique_ptr<Request>, FailureReason> tryDeserialize(
        const common::ByteSpan& data);

private:
    void appendToBuffer(const common::ByteSpan& data);

    std::optional<std::unique_ptr<Request>> deserialize(
        std::size_t messageSize);

    void eraseFromStartOfBuffer(std::size_t messageSize);

    common::ByteString m_buffer;
};
}
