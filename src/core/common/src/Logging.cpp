#include "chat/common/Logging.hpp"

#include <array>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <thread>

namespace chat::logging
{
namespace
{
Logger*& getGlobalLoggerPointer()
{
    static Logger* globalLogger = nullptr;
    return globalLogger;
}

void insertDatetime(std::ostream& out)
{
    // `std::gmtime()` is not thread-safe. The `gmtime_r()` variant is
    // thread-safe, but it is not part of the standard and thus is not
    // portable. There doesn't seem to be a way to get a simple (keyword)
    // reliable thread-safe and portable solution to this. For now, assume that
    // the function exists and acknowledge that it might cause porting issues in
    // the future.

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    gmtime_r(&time, &tm);
    out << std::put_time(&tm, "%FT%TZ");
}

void insertSeverity(std::ostream& out, Severity severity)
{
    std::string_view severityString;
    switch(severity) {
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

    constexpr int maxStringSize = 5;
    out << std::left << std::setw(maxStringSize) << severityString
        << std::right;
}
}

Logger::Logger()
  : m_out{&std::cout}
{}

void Logger::operator+=(const std::stringstream& entry)
{
    auto syncedOut = m_out.lock();
    *syncedOut.get() << entry.rdbuf() << std::endl; // Make sure to flush
}

FileLogger::FileLogger(const std::filesystem::path& logFilePath, bool truncate)
  : Logger{},
    m_logFilePath{logFilePath},
    m_fout{m_logFilePath,
           truncate ? std::fstream::out : std::fstream::out | std::fstream::app}
{
    if(!m_fout.is_open()) {
        throw std::runtime_error{"failed to open log file"};
    }

    auto syncedOut = m_out.lock();
    syncedOut.get() = &m_fout;
}

Logger& getGlobalLogger()
{
    static Logger initialGlobalLogger;
    if(getGlobalLoggerPointer() == nullptr) {
        getGlobalLoggerPointer() = &initialGlobalLogger;
    }

    return *getGlobalLoggerPointer();
}

void setGlobalLogger(Logger& logger)
{
    getGlobalLoggerPointer() = &logger;
}

std::stringstream prepareLogEntry(Severity severity,
                                  const std::filesystem::path& sourceFile,
                                  int sourceLine)
{
    std::stringstream entry;

    insertDatetime(entry);
    entry << ' ';
    insertSeverity(entry, severity);
    entry << ' ';
    entry << '[' << std::this_thread::get_id() << "] ";
    entry << '[' << sourceFile.filename().native() << ':' << sourceLine << ']';
    entry << ':' << ' ';

    return entry;
}
}
