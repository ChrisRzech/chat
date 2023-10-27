#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>

namespace chat::common
{

class Logging
{
    class LogEntry;
    class Logger;

public:
    Logging() = delete;

    Logging(const Logging& other) = delete;

    Logging& operator=(const Logging& other) = delete;

    Logging(Logging&& other) = delete;

    Logging& operator=(Logging&& other) = delete;

    ~Logging() = default;

    enum Severity
    {
        Fatal,
        Error,
        Warn,
        Info,
        Debug
    };

    static void initialize(bool allowDebug, const std::filesystem::path& logFile);

    static bool canLog(Severity severity);

    static Logger& getLogger();

    static LogEntry buildLogEntry(Severity severity, const std::filesystem::path& sourceFile, uint32_t line);

private:
    class Logger
    {
    public:
        Logger(bool allowDebug, const std::filesystem::path& logFile);

        Logger(const Logger& logger) = delete;

        Logger& operator=(const Logger& logger) = delete;

        Logger(Logger&& logger) = delete;

        Logger& operator=(Logger&& logger) = delete;

        ~Logger() = default;

        bool isDebugAllowed();

        void operator+=(const LogEntry& logEntry);

    private:
        const bool m_debugEnabled;
        std::mutex m_mutex;
        std::fstream m_out;
    };

    class LogEntry
    {
    public:
        LogEntry(Severity severity, const std::filesystem::path& sourceFile, uint32_t line);

        LogEntry(const LogEntry& logger) = delete;

        LogEntry& operator=(const LogEntry& logger) = delete;

        LogEntry(LogEntry&& logger) = delete;

        LogEntry& operator=(LogEntry&& logger) = delete;

        ~LogEntry() = default;

        template<typename T>
        LogEntry& operator<<(const T& object)
        {
            m_builder << object;
            return *this;
        }

        std::string string() const;

    private:
        std::stringstream m_builder;
    };

    inline static std::unique_ptr<Logger> s_logger;
};

#define LOG(severity) if(!chat::common::Logging::canLog(severity)) {} else \
    chat::common::Logging::getLogger() += chat::common::Logging::buildLogEntry(severity, __FILE__, __LINE__)
#define LOG_FATAL LOG(chat::common::Logging::Severity::Fatal)
#define LOG_ERROR LOG(chat::common::Logging::Severity::Error)
#define LOG_WARN LOG(chat::common::Logging::Severity::Warn)
#define LOG_INFO LOG(chat::common::Logging::Severity::Info)
#define LOG_DEBUG LOG(chat::common::Logging::Severity::Debug)

}
