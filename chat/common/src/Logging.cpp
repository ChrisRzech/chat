#include "chat/common/Logging.hpp"

#include <string_view>
#include <thread>

namespace chat::common
{

void Logging::initialize(bool allowDebug, const std::filesystem::path& logFile)
{
    s_logger = std::make_unique<Logger>(allowDebug, logFile);
}

bool Logging::canLog(Severity severity)
{
    return (severity == Severity::Debug && s_logger->isDebugAllowed()) || severity != Severity::Debug;
}

Logging::Logger& Logging::getLogger()
{
    return *s_logger;
}

Logging::LogEntry Logging::buildLogEntry(Severity severity, const std::filesystem::path& sourceFile, uint32_t line)
{
    return LogEntry{severity, sourceFile, line};
}

Logging::Logger::Logger(bool allowDebug, const std::filesystem::path& logFilepath)
  : m_debugEnabled{allowDebug},
    m_mutex{},
    m_out{logFilepath, std::fstream::out | std::fstream::app}
{}

bool Logging::Logger::isDebugAllowed()
{
    return m_debugEnabled;
}

void Logging::Logger::operator+=(const LogEntry& logEntry)
{
    auto logEntryString = logEntry.string();

    std::unique_lock lock(m_mutex);
    m_out << logEntryString << std::endl; //Make sure to flush
}

Logging::LogEntry::LogEntry(Severity severity, const std::filesystem::path& sourceFile, uint32_t line)
  : m_builder{}
{
    std::string_view severityString;
    switch(severity)
    {
    case Severity::Fatal:
        severityString = "FATAL";
        break;
    case Severity::Error:
        severityString = "ERROR";
        break;
    case Severity::Warn:
        severityString = "WARN";
        break;
    case Severity::Info:
        severityString = "INFO";
        break;
    case Severity::Debug:
        severityString = "DEBUG";
        break;
    }

    auto threadId = std::this_thread::get_id();

    //`yyyy-mm-dd hh:mm:ss.ms` using GMT timezone
    auto time = std::chrono::system_clock::now();
    auto cTime = std::chrono::system_clock::to_time_t(time);
    auto calendar = *std::gmtime(&cTime);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()) % 1000;

    m_builder << std::setfill('0')
        << calendar.tm_year + 1900 << '-'
        << std::setw(2) << calendar.tm_mon + 1 << '-'
        << std::setw(2) << calendar.tm_mday << ' '
        << std::setw(2) << calendar.tm_hour << ':'
        << std::setw(2) << calendar.tm_min << ':'
        << std::setw(2) << calendar.tm_sec << '.'
        << std::setw(3) << milliseconds.count()
        << std::setfill(' ')  << ' '
        << std::left << std::setw(5) << severityString << ' '
        << '[' << threadId << ']' << ' '
        << '[' << sourceFile.filename().string() << ':' << line << ']' << ':' << ' ';
}

std::string Logging::LogEntry::string() const
{
    return m_builder.str();
}

}
