#include "chat/messages/Close.hpp"

namespace chat::messages
{

Close::Close()
  : Message{Type::Close}
{}

void Close::serialize([[maybe_unused]] common::OutputByteStream& stream) const
{
    Message::serialize(stream);
    // No data to insert
}

bool Close::deserialize([[maybe_unused]] common::InputByteStream& stream)
{
    // No data to extract
    return true;
}

}
