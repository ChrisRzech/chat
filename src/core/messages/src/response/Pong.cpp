#include "chat/messages/response/Pong.hpp"

namespace chat::messages
{

Pong::Pong()
  : Response{Type::Pong}
{}

void Pong::serialize(common::OutputByteStream& stream) const
{
    Response::serialize(stream);
    // No data to insert
}

bool Pong::deserialize([[maybe_unused]] common::InputByteStream& stream)
{
    // No data to extract
    return true;
}

}
