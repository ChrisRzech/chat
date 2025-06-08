#include "chat/messages/request/Ping.hpp"

#include "chat/common/InputByteStream.hpp"
#include "chat/common/OutputByteStream.hpp"
#include "chat/messages/Request.hpp"

namespace chat::messages
{

Ping::Ping()
  : Request{Type::Ping}
{}

void Ping::serialize(common::OutputByteStream& stream) const
{
    Request::serialize(stream);
    // No data to insert
}

bool Ping::deserialize([[maybe_unused]] common::InputByteStream& stream)
{
    // No data to extract
    return true;
}

}
