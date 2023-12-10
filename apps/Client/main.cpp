#include "chat/client/Client.hpp"

#include "chat/common/Logging.hpp"

int main()
{
    //Initializing logging is not done inside of the try-catch since the catch blocks attempt to log
    const auto logFilepath = "client.log";
    chat::common::Logging::initialize(logFilepath, true);

    try
    {
        chat::client::Client client{"localhost", 25565};
        auto ping = client.ping();
        if(ping.has_value())
        {
            LOG_DEBUG << "Ping: " << ping.value().count() << "ms";
        }
        else
        {
            LOG_DEBUG << "Ping failed";
        }
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
