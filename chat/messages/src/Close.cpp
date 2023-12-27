#include "chat/messages/Close.hpp"

namespace chat::messages
{

Close::Close()
  : Message{Type::Close}
{}

void Close::serialize([[maybe_unused]] sf::Packet& packet) const
{
    Message::serialize(packet);
    //No data to insert
}

bool Close::deserialize([[maybe_unused]] sf::Packet& packet)
{
    //No data to extract
    return true;
}

}
