#include "chat/messages/request/Ping.hpp"

namespace chat::messages
{

Ping::Ping()
  : Request{Type::Ping}
{}

void Ping::serialize(sf::Packet& packet) const
{
    Request::serialize(packet);
    //No data to insert
}

bool Ping::deserialize([[maybe_unused]] sf::Packet& packet)
{
    //No data to extract
    return true;
}

}
