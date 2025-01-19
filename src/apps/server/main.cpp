#include "chat/common/Logging.hpp"

#include "chat/server/Server.hpp"

int main()
{
    try {
        const auto logFilepath = "server.log";
        chat::common::Logging::enableLoggingToFile(logFilepath, true);

        constexpr chat::common::Port PORT{25565};
        constexpr int MAX_THREAD_COUNT = 4;
        chat::server::Server server(PORT, MAX_THREAD_COUNT);
        server.run();
    } catch(const std::exception& exception) {
        LOG_FATAL << "Exception caught: " << exception.what();
        return 1;
    } catch(...) {
        LOG_FATAL << "Unknown exception!";
        return 1;
    }

    return 0;
}
