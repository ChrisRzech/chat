#pragma once

#include "chat/common/Synced.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <ostream>
#include <source_location>
#include <string>

namespace chat::logging
{
/**
 * @brief The severity of a log entry.
 *
 * @details A @c Severity is provided when logging an entry. It simply
 * determines the string value for the severity column in the log entry.
 *
 * However, a log entry with a @c Debug severity only logs if debug logging is
 * enabled (see @c enableDebugLogging). Otherwise, the log entry is elided at
 * compile-time.
 */
enum class Severity
{
    Fatal,
    Error,
    Warn,
    Info,
    Debug
};

/**
 * @brief Create a log entry that is prepared with initial information.
 *
 * @param severity The severity of the entry.
 *
 * @param location The location in the source code of where the log occurs.
 *
 * @return The log entry.
 */
[[nodiscard]] std::stringstream prepareLogEntry(
    Severity severity, const std::source_location& location);

/**
 * @brief A type to log entries into a stream.
 *
 * @details @c Logger holds a pointer to an `std::ostream`. When a log is
 * performed, the entry is inserted into the `std::ostream` in a thread-safe
 * manner.
 *
 * The default constructor of @c Logger uses `std::cout` as the output stream.
 * To use a different stream, a new type should be created that derives from
 * @c Logger that sets the `std::ostream` pointer.
 *
 * It is expected to use the `LOG()` or `LOG_*()` macros when logging.
 */
class Logger
{
public:
    /**
     * @brief Construct a logger.
     *
     * @details The logger logs to `std::cout`.
     */
    Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    /**
     * @brief Destroy the logger.
     */
    virtual ~Logger() = default;

    /**
     * @brief Log an entry.
     *
     * @details This function is thread-safe.
     *
     * @tparam Args The types of the format arguments.
     *
     * @param severity The severity of the entry.
     *
     * @param location The location in the source code of where the log occurs.
     *
     * @param format The format string.
     *
     * @param args The arguments for the format.
     */
    template<typename... Args>
    void log(Severity severity, const std::source_location& location,
             std::format_string<Args...> format, Args&&... args)
    {
        auto entry = prepareLogEntry(severity, location);
        entry << std::format(format, std::forward<Args>(args)...);
        auto syncedOut = m_out.lock();
        *syncedOut.get() << entry.rdbuf() << std::endl; // Make sure to flush
    }

protected:
    common::Synced<std::ostream*> m_out;
};

/**
 * @brief A @c Logger that logs to a file.
 */
class FileLogger : public Logger
{
public:
    /**
     * @brief Construct a file logger.
     *
     * @param logFilePath The path to a file to log all log entries.
     *
     * @param truncate True if the log file should be truncated when opened;
     * otherwise, the file is not truncated when opened.
     */
    FileLogger(const std::filesystem::path& logFilePath, bool truncate);

private:
    std::filesystem::path m_logFilePath;
    std::fstream m_fout;
};

/**
 * @brief Determines if the debug severity is to be logged. Usually, the value
 * is determined by the project build mode.
 */
static constexpr bool enableDebugLogging = true;

/**
 * @brief Get the global logger.
 *
 * @details This funtion returns a reference using a static pointer. The
 * returned reference can be changed by using @c setGlobalLogger(). On program
 * initialization, the static pointer points to `nullptr`. If this function is
 * called and the static pointer is `nullptr`, the pointer is set to point to a
 * static @c Logger is default constructed.
 *
 * This function is not thread-safe.
 *
 * @return The global logger.
 */
[[nodiscard]] Logger& getGlobalLogger();

/**
 * @brief Set the global logger.
 *
 * @details A pointer to the provided object is stored in a static pointer.
 *
 * This function is not thread-safe.
 *
 * @param logger The logger.
 */
void setGlobalLogger(Logger& logger);

/**
 * @brief Determine if the severity should be logged.
 *
 * @param severity The severity of the log entry.
 *
 * @return True if the severity should be logged; otherwise, false.
 */
[[nodiscard]] constexpr bool shouldLog(Severity severity)
{
    return severity == Severity::Debug ? enableDebugLogging : true;
}

/**
 * @brief A macro for performing logging.
 */
#define LOG(severity, format, ...)                     \
    if constexpr(chat::logging::shouldLog(severity)) { \
        chat::logging::getGlobalLogger().log(          \
            severity, std::source_location::current(), \
            format __VA_OPT__(, __VA_ARGS__));         \
    }

#define LOG_FATAL(format, ...) \
    LOG(chat::logging::Severity::Fatal, format __VA_OPT__(, __VA_ARGS__))
#define LOG_ERROR(format, ...) \
    LOG(chat::logging::Severity::Error, format __VA_OPT__(, __VA_ARGS__))
#define LOG_WARN(format, ...) \
    LOG(chat::logging::Severity::Warn, format __VA_OPT__(, __VA_ARGS__))
#define LOG_INFO(format, ...) \
    LOG(chat::logging::Severity::Info, format __VA_OPT__(, __VA_ARGS__))
#define LOG_DEBUG(format, ...) \
    LOG(chat::logging::Severity::Debug, format __VA_OPT__(, __VA_ARGS__))
}
