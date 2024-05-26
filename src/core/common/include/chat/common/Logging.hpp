#pragma once

#include "chat/common/SynchronizedObject.hpp"

#include <filesystem>
#include <fstream>
#include <optional>
#include <ostream>
#include <sstream>

namespace chat::common
{

/**
 * @brief Contains all logging functionality.
 *
 * @details Log entries are outputted to a file with a specific format in a
 * thread-safe manner.
 *
 * By default, logging is outputted to @c std::cout. This behavior can be
 * changed to using files instead by using @c enableLoggingToFile(). Using
 * @c disableLoggingToFile() reverts the behavior to the default behavior.
 *
 * A severity is needed when creating a log entry. The severity only determines
 * the string value for the severity column in the log entry. However, log
 * entries with a debug severity will only log if debug logging is enabled;
 * otherwise, the log entry is elided at compile-time.
 *
 * In order to log objects, use the `LOG()` or `LOG_X()` macros followed by a
 * sequence of insertion operators (`operator<<`). The object following the
 * insertion operator must have an overload for `operator<<` with an
 * `std::ostream`.
 */
class Logging
{
    class LogEntry;
    class Logger;

public:
    /**
     * @brief Severity for log entries.
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
     * @brief Constructor is disabled.
     */
    Logging() = delete;

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Logging(const Logging& other) = delete;
    Logging& operator=(const Logging& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Logging(Logging&& other) = delete;
    Logging& operator=(Logging&& other) = delete;
    /** @} */

    /**
     * @brief Destroy logging object.
     */
    ~Logging() = default;

    /**
     * @brief Enable logging to a file.
     *
     * @param logFile The path to a file to log all log entries.
     *
     * @param truncate True if the log file should be truncated when opened;
     * otherwise, the file will not be truncated when opened.
     */
    static void enableLoggingToFile(std::filesystem::path logFile,
                                    bool truncate);

    /**
     * @brief Disable logging to a file.
     */
    static void disableLoggingToFile();

    /**
     * @brief Determine if a log entry should be logged.
     *
     * @param severity The severity of the log entry.
     *
     * @return True if the log entry should be logged; otherwise, false.
     */
    [[nodiscard]] static constexpr bool shouldLog(Severity severity)
    {
        return severity == Severity::Debug ? ENABLE_LOGGING_DEBUG_SEVERITY
                                           : true;
    }

    /**
     * @brief Get the global logger used for all logging.
     *
     * @return The global logger used for all logging.
     */
    [[nodiscard]] static Logger& getLogger();

    /**
     * @brief Create a new log entry.
     *
     * @details The log entry should be filled with items using
     * @c LogEntry::operator<<; otherwise, the log entry will only contain a
     * header with no information.
     *
     * @param severity The severity of the log entry.
     *
     * @param sourceFile The source file that the log entry is being created in.
     * Usually, the value is `__FILE__`.
     *
     * @param line The line number of the source file that the log entry was
     * created at. Usually, the value is `__LINE__`.
     *
     * @return A new log entry.
     */
    [[nodiscard]] static LogEntry createLogEntry(
        Severity severity, const std::filesystem::path& sourceFile,
        int line);

    /**
     * @brief Determines if debug severity will log a log entry. Usually, the
     * value is determined by the project build mode.
     */
    static constexpr bool ENABLE_LOGGING_DEBUG_SEVERITY = true;

private:
    /**
     * @brief Handles all logic for logging such as logging a @c LogEntry and
     * file rotation.
     *
     * @details By default, the logger outputs to @c std::cout. Using
     * @c enableLoggingToFile() will make the logger output to files instead.
     */
    class Logger
    {
    public:
        /**
         * @brief Construct a logger.
         */
        Logger();

        /**
         * @brief Copy operations are disabled.
         * @{
         */
        Logger(const Logger& other) = delete;
        Logger& operator=(const Logger& other) = delete;
        /** @} */

        /**
         * @brief Move operations are disabled.
         * @{
         */
        Logger(Logger&& other) = delete;
        Logger& operator=(Logger&& other) = delete;
        /** @} */

        /**
         * @brief Destroy the logger.
         */
        ~Logger() = default;

        /**
         * @brief Enable logging to a file.
         *
         * @param logFile The path to a file to log all log entries.
         *
         * @param truncate True if the log file should be truncated when opened;
         * otherwise, the file will not be truncated when opened.
         */
        void enableLoggingToFile(std::filesystem::path logFile, bool truncate);

        /**
         * @brief Disable logging to a file.
         */
        void disableLoggingToFile();

        /**
         * @brief Log a log entry.
         *
         * @details This function is thread-safe. The reasoning behind using
         * `operator+=` is to allow this function to be called after the log
         * entry has been fully built. Any operator could work as long as it is
         * overloadable and does not take precedence over `operator<<`, which is
         * used for building the @c LogEntry.
         *
         * @param logEntry Log entry to be logged.
         */
        void operator+=(const LogEntry& logEntry);

    private:
        SynchronizedObject<std::ostream*> m_out;
        std::optional<std::filesystem::path> m_logFile;
        std::unique_ptr<std::fstream> m_fout;
    };

    /**
     * @brief An object that is built upon that will be logged.
     */
    class LogEntry
    {
    public:
        /**
         * @brief Construct a log entry.
         */
        LogEntry() = default;

        /**
         * @brief Copy operations are disabled.
         * @{
         */
        LogEntry(const LogEntry& other) = delete;
        LogEntry& operator=(const LogEntry& other) = delete;
        /** @} */

        /**
         * @brief Move operations are enabled.
         * @{
         */
        LogEntry(LogEntry&& other) = default;
        LogEntry& operator=(LogEntry&& other) = default;
        /** @} */

        /**
         * @brief Destroy the log entry.
         */
        ~LogEntry() = default;

        /**
         * @brief Insert an object into the log entry.
         *
         * @details This is the function to use when needing to log objects.
         * Operator chaining is supported to allow easily insert multiple
         * objects.
         *
         * @tparam T The type of the object. This type must have an operator
         * overload for `operator<<` with an `std::ostream`.
         *
         * @param object The object to insert.
         *
         * @return This object to support operator chaining.
         */
        template<typename T>
        LogEntry& operator<<(const T& object)
        {
            m_builder << object;
            return *this;
        }

        /**
         * @brief Get a string conversion of the log entry.
         *
         * @return A string conversion of the log entry.
         */
        [[nodiscard]] std::string toString() const;

    private:
        std::stringstream m_builder;
    };
};

#define LOG(severity)                                                 \
    if constexpr(!chat::common::Logging::shouldLog(severity)) {       \
    } else                                                            \
        chat::common::Logging::getLogger() +=                         \
            chat::common::Logging::createLogEntry(severity, __FILE__, \
                                                  __LINE__)
#define LOG_FATAL LOG(chat::common::Logging::Severity::Fatal)
#define LOG_ERROR LOG(chat::common::Logging::Severity::Error)
#define LOG_WARN LOG(chat::common::Logging::Severity::Warn)
#define LOG_INFO LOG(chat::common::Logging::Severity::Info)
#define LOG_DEBUG LOG(chat::common::Logging::Severity::Debug)

}
