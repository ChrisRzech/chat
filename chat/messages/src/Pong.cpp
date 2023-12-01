#include "chat/messages/Pong.hpp"

namespace chat::messages
{

Pong::Pong()
  : Response{Type::Pong}
{}

void Pong::toPacket(sf::Packet& packet) const
{
    Response::toPacket(packet);
    //No data to insert
}

bool Pong::fromPacket([[maybe_unused]] sf::Packet& packet)
{
    //No data to extract
    return true;
}

}
