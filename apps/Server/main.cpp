#include "chat/common/Logging.hpp"

#include "chat/server/Server.hpp"

int main()
{
    //Initializing logging is not done inside of the try-catch since the catch blocks attempt to log
    const auto logFilepath = "server.log";
    chat::common::Logging::initialize(logFilepath, true);

    try
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
    }
    catch(const std::exception& exception)
    {
        LOG_ERROR << exception.what();
        return 1;
    }
    catch(...)
    {
        LOG_ERROR << "Unknown exception!";
        return 1;
    }

    return 0;
}
