#include "chat/common/Logging.hpp"

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

std::tm getCalendar(const time_t& timer)
{
    // `std::gmtime()` is not thread safe while `::gmtime_r()` should be. The
    // only concern is that `::gmtime_r()` might not be portable.
    std::tm calendar{};
    auto result = ::gmtime_r(&timer, &calendar);
    return result != nullptr ? *result : std::tm{};
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

    auto time = std::chrono::system_clock::now();
    auto cTime = std::chrono::system_clock::to_time_t(time);
    auto calendar = getCalendar(cTime);
    constexpr int MILLISECONDS_IN_SECOND = 1000;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                            time.time_since_epoch()) %
                        MILLISECONDS_IN_SECOND;

    // Format `yyyy-mm-dd hh:mm:ss.ms` and GMT timezone
    constexpr int START_YEAR = 1900;
    entry << std::setfill('0');
    entry << calendar.tm_year + START_YEAR << '-';
    entry << std::setw(2) << calendar.tm_mon + 1 << '-';
    entry << std::setw(2) << calendar.tm_mday << ' ';
    entry << std::setw(2) << calendar.tm_hour << ':';
    entry << std::setw(2) << calendar.tm_min << ':';
    entry << std::setw(2) << calendar.tm_sec << '.';
    entry << std::setw(3) << milliseconds.count();
    entry << std::setfill(' ');
    entry << ' ';

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
    constexpr int SEVERITY_WIDTH = 5;
    entry << std::left;
    entry << std::setw(SEVERITY_WIDTH) << severityString;
    entry << std::right;
    entry << ' ';

    auto threadId = std::this_thread::get_id();
    entry << '[' << threadId << ']';
    entry << ' ';

    entry << '[' << sourceFile.filename().native() << ':' << sourceLine << ']';
    entry << ':' << ' ';

    return entry;
}
}
