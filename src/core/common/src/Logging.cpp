#include "chat/common/Logging.hpp"

#include <ctime>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <thread>

namespace chat::common
{

namespace
{

std::tm getCalendar(const time_t& timer)
{
    // `std::gmtime()` is not thread safe while `::gmtime_r()` should be. The
    // only concern is that `::gmtime_r()` might not be portable.
    std::tm calendar{};
    auto result = ::gmtime_r(&timer, &calendar);
    return result != nullptr ? *result : std::tm{};
}

}

void Logging::enableLoggingToFile(std::filesystem::path logFile, bool truncate)
{
    getLogger().enableLoggingToFile(std::move(logFile), truncate);
}

void Logging::disableLoggingToFile()
{
    getLogger().disableLoggingToFile();
}

Logging::Logger& Logging::getLogger()
{
    static Logger logger;
    return logger;
}

Logging::LogEntry Logging::createLogEntry(
    Severity severity, const std::filesystem::path& sourceFile, int line)
{
    LogEntry entry;

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

    entry << '[' << sourceFile.filename().string() << ':' << line << ']';
    entry << ':' << ' ';

    return entry;
}

Logging::Logger::Logger()
  : m_out{&std::cout},
    m_logFile{},
    m_fout{}
{}

void Logging::Logger::enableLoggingToFile(std::filesystem::path logFile,
                                          bool truncate)
{
    m_logFile = std::move(logFile);
    m_fout = std::make_unique<std::fstream>(
        m_logFile.value(),
        truncate ? std::fstream::out : std::fstream::out | std::fstream::app);

    if(!m_fout->is_open()) {
        m_logFile.reset();
        m_fout.reset();
        throw std::invalid_argument{"Could not open log file"};
    }

    auto lockedOut = m_out.lock();
    lockedOut.get() = m_fout.get();
}

void Logging::Logger::disableLoggingToFile()
{
    m_logFile.reset();
    m_fout.reset();

    auto lockedOut = m_out.lock();
    lockedOut.get() = &std::cout;
}

void Logging::Logger::operator+=(const LogEntry& logEntry)
{
    // No need to hold the lock while converting to a string
    auto logEntryString = logEntry.toString();

    auto lockedOut = m_out.lock();
    *lockedOut.get() << logEntryString << std::endl; // Make sure to flush
}

std::string Logging::LogEntry::toString() const
{
    return m_builder.str();
}

}
