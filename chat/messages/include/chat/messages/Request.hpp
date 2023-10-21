#pragma once

#include "chat/messages/Message.hpp"

namespace chat::messages
{

class Request : public Message
{
protected:
    Request();
};

}
