#include "chat/client/Client.hpp"
#include "chat/common/Logging.hpp"
#include "chat/common/Port.hpp"

#include <exception>

int main()
{
    try {
        const auto logFilepath = "client.log";
        chat::logging::FileLogger logger{logFilepath, true};
        chat::logging::setGlobalLogger(logger);

        constexpr chat::common::Port PORT{25565};
        chat::client::Client client{"localhost", PORT};
        auto ping = client.ping();
        if(ping.has_value()) {
            LOG_DEBUG("Ping: {}ms", ping.value().count());
        } else {
            LOG_DEBUG("Ping failed");
        }
    } catch(const std::exception& exception) {
        LOG_FATAL("Exception caught: {}", exception.what());
        return 1;
    } catch(...) {
        LOG_FATAL("Unknown exception!");
        return 1;
    }

    return 0;
}
