#include "chat/messages/Message.hpp"

namespace chat::messages
{

Message::Message(Type type)
  : m_type{type}
{}

Message::Type Message::getMessageType() const
{
    return m_type;
}

void Message::serialize(common::OutputByteStream& stream) const
{
    stream << std::underlying_type_t<Type>(m_type);
}

}
