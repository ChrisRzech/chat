#include "chat/messages/Close.hpp"

namespace chat::messages
{

Close::Close()
  : Message{Type::Close}
{}

bool Close::fromPacket(sf::Packet& packet)
{
    std::ignore = packet; //Nothing to extract from packet
    return true;
}

}
