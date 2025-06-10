#pragma once

#include "chat/common/BufferView.hpp"
#include "chat/common/Result.hpp"
#include "chat/messages/Request.hpp"

#include <cstdint>
#include <memory>

namespace chat::messages
{
class IncrementalRequestDeserializer
{
public:
    enum class FailureReason : std::uint8_t
    {
        Partial,
        Error,
    };

    common::Result<std::unique_ptr<Request>, FailureReason> tryDeserialize(
        const common::BufferView& data);

private:
    void appendToBuffer(const common::BufferView& data);

    std::optional<std::unique_ptr<Request>> deserialize(
        std::size_t messageSize);

    void eraseFromStartOfBuffer(std::size_t messageSize);

    common::Buffer m_buffer;
};
}
