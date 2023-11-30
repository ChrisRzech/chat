#include "chat/client/Client.hpp"

#include "chat/common/Logging.hpp"

int main()
{
    try
    {
        constexpr bool DEBUG_LOGGING_ALLOWED = true; //TODO Change value based off of project build mode (debug vs release)
        const auto logFilepath = "client.log";
        chat::common::Logging::initialize(DEBUG_LOGGING_ALLOWED, logFilepath);

        chat::client::Client client{"localhost", 25565};
        auto testResponse = client.ping("ping");
        if(testResponse.has_value())
        {
            LOG_DEBUG << "Response: " << testResponse.value();
        }
        else
        {
            LOG_DEBUG << "Bad response";
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
