#include "chat/common/Logging.hpp"
#include "chat/common/Port.hpp"
#include "chat/server/Server.hpp"

#include <cstddef>
#include <exception>
#include <filesystem>
#include <iterator>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

struct Options
{
    std::optional<std::filesystem::path> logFilePath;
    chat::common::Port port{25565};
    std::size_t maxThreadCount = 2;
};

Options parseOptions(int argc, char* argv[])
{
    Options options;

    const std::vector<std::string> args{argv, std::next(argv, argc)};
    for(std::size_t i = 1; i < args.size(); i++) {
        const auto& arg = args.at(i);
        if(arg == "--log-file") {
            options.logFilePath = args.at(i + 1);
            i++;
        } else if(arg == "--port") {
            options.port = chat::common::Port{
                static_cast<std::underlying_type_t<chat::common::Port>>(
                    std::stoi(args.at(i + 1)))};
            i++;
        } else if(arg == "--max-thread-count") {
            options.maxThreadCount = std::stoi(args.at(i + 1));
            i++;
        } else {
            throw std::invalid_argument{"unexpected argument"};
        }
    }

    return options;
}

int main(int argc, char* argv[])
{
    try {
        auto options = parseOptions(argc, argv);

        std::optional<chat::logging::FileLogger> fileLogger;
        if(options.logFilePath.has_value()) {
            chat::logging::setGlobalLogger(
                fileLogger.emplace(options.logFilePath.value(), true));
        }

        chat::server::Server server(options.port, options.maxThreadCount);
        server.run();
    } catch(const std::exception& exception) {
        LOG_FATAL("Exception caught: {}", exception.what());
        return 1;
    } catch(...) {
        LOG_FATAL("Unknown exception!");
        return 1;
    }

    return 0;
}
