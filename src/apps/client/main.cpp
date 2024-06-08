#include "chat/client/Client.hpp"

#include "chat/common/Logging.hpp"

int main()
{
    try {
        const auto logFilepath = "client.log";
        chat::common::Logging::enableLoggingToFile(logFilepath, true);

        constexpr std::uint16_t PORT = 25565;
        chat::client::Client client{"localhost", PORT};
        auto ping = client.ping();
        if(ping.has_value()) {
            LOG_DEBUG << "Ping: " << ping.value().count() << "ms";
        } else {
            LOG_DEBUG << "Ping failed";
        }
    } catch(const std::exception& exception) {
        LOG_FATAL << exception.what();
        return 1;
    } catch(...) {
        LOG_FATAL << "Unknown exception!";
        return 1;
    }

    return 0;
}
