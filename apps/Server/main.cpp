#include "chat/server/Server.hpp"

int main()
{
    constexpr uint16_t PORT = 25565;
    constexpr uint16_t MAX_THREAD_COUNT = 4;
    chat::server::Server server(PORT, MAX_THREAD_COUNT);
    server.start();
    bool stopping = false;
    while(!stopping)
    {
        //TODO Wait for something to stop the server
    }
    server.stop();
    return 0;
}
