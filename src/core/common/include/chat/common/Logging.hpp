#pragma once

#include "chat/common/Synced.hpp"

#include <filesystem>
#include <fstream>
#include <ostream>
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

    Logger(const Logger&) = default;
    Logger& operator=(const Logger&) = default;
    Logger(Logger&&) = default;
    Logger& operator=(Logger&&) = default;

    /**
     * @brief Destroy the logger.
     */
    virtual ~Logger() = default;

    /**
     * @brief Log an entry.
     *
     * @details The reasoning behind using `operator+=` is to allow this
     * function to be called after the log entry has been fully built. Any
     * operator could work as long as it is overloadable and does not take
     * precedence over `operator<<`, which is used for building the @c LogEntry.
     *
     * This function is thread-safe.
     *
     * @param entry The stream representing an entry.
     */
    void operator+=(const std::stringstream& entry);

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
 * @brief Create a log entry that is prepared with initial information.
 *
 * @param severity The severity of the entry.
 *
 * @param sourceFile The source file that the log entry is being created in.
 * Usually, the value is `__FILE__`.

 * @param sourceLine The line number of the source file that the log entry was
 * created at. Usually, the value is `__LINE__`.
 *
 * @return The log entry.
 */
[[nodiscard]] std::stringstream prepareLogEntry(
    Severity severity, const std::filesystem::path& sourceFile, int sourceLine);

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
 *
 * @details After calling this macro, objects can be added to the log entry
 * by using the insertion operator (`operator<<`) for each object to log. The
 * object being logged must have an overload for `operator<<` with an
 * `std::ostream`.
 */
#define LOG(severity)                                   \
    if constexpr(!chat::logging::shouldLog(severity)) { \
    } else                                              \
        chat::logging::getGlobalLogger() +=             \
            chat::logging::prepareLogEntry(severity, __FILE__, __LINE__)

#define LOG_FATAL LOG(chat::logging::Severity::Fatal)
#define LOG_ERROR LOG(chat::logging::Severity::Error)
#define LOG_WARN LOG(chat::logging::Severity::Warn)
#define LOG_INFO LOG(chat::logging::Severity::Info)
#define LOG_DEBUG LOG(chat::logging::Severity::Debug)
}
