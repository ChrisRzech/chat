#include "chat/messages/Close.hpp"

namespace chat::messages
{

Close::Close()
  : Message{Type::Close}
{}

void Close::toPacket([[maybe_unused]] sf::Packet& packet) const
{
    //No data to insert
}

bool Close::fromPacket([[maybe_unused]] sf::Packet& packet)
{
    //No data to extract
    return true;
}

}
