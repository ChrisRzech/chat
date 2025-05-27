#include "chat/messages/IncrementalRequestDeserializer.hpp"

#include "chat/messages/serialize.hpp"

namespace chat::messages
{
common::Result<std::unique_ptr<Request>,
               IncrementalRequestDeserializer::FailureReason>
IncrementalRequestDeserializer::tryDeserialize(const common::BufferView& data)
{
    // This function can be optimized a lot more such saving the message size as
    // a member of the class. However, keep it simple for now since the
    // implementation does not affect the behavior of the function.

    common::Result<std::unique_ptr<Request>, FailureReason> result{
        common::Error{FailureReason::Error}};

    appendToBuffer(data);

    const common::BufferView bufferView{m_buffer.data(), m_buffer.size()};
    common::InputByteStream bufferStream{bufferView};

    std::uint32_t messageSize = 0;
    if(bufferStream >> messageSize) {
        const std::size_t fullMessageSize = sizeof(messageSize) + messageSize;
        if(m_buffer.size() >= fullMessageSize) {
            auto request = deserialize(fullMessageSize);
            if(request.has_value()) {
                result = std::move(request.value());
            } else {
                result = common::Error{FailureReason::Error};
            }
        } else {
            result = common::Error{FailureReason::Partial};
        }
    } else {
        result = common::Error{FailureReason::Partial};
    }

    return result;
}

void IncrementalRequestDeserializer::appendToBuffer(
    const common::BufferView& data)
{
    m_buffer.insert(m_buffer.end(), data.begin(), data.end());
}

std::optional<std::unique_ptr<Request>>
IncrementalRequestDeserializer::deserialize(std::size_t messageSize)
{
    const common::BufferView serialized{m_buffer.data(), messageSize};
    auto request = messages::deserializeRequest(serialized);
    eraseFromStartOfBuffer(messageSize);
    return request;
}

void IncrementalRequestDeserializer::eraseFromStartOfBuffer(
    std::size_t messageSize)
{
    const auto endIt = std::next(m_buffer.begin(), messageSize);
    m_buffer.erase(m_buffer.begin(), endIt);
}
}
