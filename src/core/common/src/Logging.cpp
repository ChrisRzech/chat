#include "chat/common/Logging.hpp"

#include <iostream>
#include <stdexcept>
#include <string_view>
#include <thread>

namespace chat::common
{

void Logging::enableLoggingToFile(std::filesystem::path logFile, bool truncate)
{
    s_logger.enableLoggingToFile(std::move(logFile), truncate);
}

void Logging::disableLoggingToFile()
{
    s_logger.disableLoggingToFile();
}

Logging::Logger& Logging::getLogger()
{
    return s_logger;
}

Logging::LogEntry Logging::createLogEntry(Severity severity, const std::filesystem::path& sourceFile, uint32_t line)
{
    LogEntry entry;

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

    auto time = std::chrono::system_clock::now();
    auto cTime = std::chrono::system_clock::to_time_t(time);
    auto calendar = *std::gmtime(&cTime);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()) % 1000;

    //`yyyy-mm-dd hh:mm:ss.ms` using GMT timezone
    entry << std::setfill('0')
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

    return entry;
}

Logging::Logger::Logger()
  : m_out{&std::cout},
    m_logFile{},
    m_fout{}
{}

void Logging::Logger::enableLoggingToFile(std::filesystem::path logFile, bool truncate)
{
    m_logFile = std::move(logFile);
    m_fout = std::make_unique<std::fstream>(m_logFile.value(), truncate ? std::fstream::out : std::fstream::out | std::fstream::app);

    if(!m_fout->is_open())
    {
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
    auto logEntryString = logEntry.toString(); //No need to hold the lock while converting to a string

    auto lockedOut = m_out.lock();
    *lockedOut.get() << logEntryString << std::endl; //Make sure to flush
}

std::string Logging::LogEntry::toString() const
{
    return m_builder.str();
}

}
