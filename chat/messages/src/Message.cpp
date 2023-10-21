#include "chat/messages/Message.hpp"

namespace chat::messages
{

Message::Message(Type type)
  : m_type{type}
{}

Type Message::getType() const
{
    return m_type;
}

}
