#include "chat/messages/Ping.hpp"

namespace chat::messages
{

Ping::Ping()
  : Request{Type::Ping}
{}

void Ping::toPacket(sf::Packet& packet) const
{
    Request::toPacket(packet);
    //No data to insert
}

bool Ping::fromPacket([[maybe_unused]] sf::Packet& packet)
{
    //No data to extract
    return true;
}

}
