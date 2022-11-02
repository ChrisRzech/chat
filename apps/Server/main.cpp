#include "chat/chatserver/ChatServer.hpp"

int main()
{
    chat::ChatServer server(25565);
    server.run();
}
