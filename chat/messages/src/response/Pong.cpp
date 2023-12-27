#include "chat/messages/response/Pong.hpp"

namespace chat::messages
{

Pong::Pong()
  : Response{Type::Pong}
{}

void Pong::serialize(sf::Packet& packet) const
{
    Response::serialize(packet);
    //No data to insert
}

bool Pong::deserialize([[maybe_unused]] sf::Packet& packet)
{
    //No data to extract
    return true;
}

}
