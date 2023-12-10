#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>

namespace chat::common
{

/**
 * @brief Contains all logging functionality.
 *
 * @details Log entries are outputted to a file with a specific format in a thread-safe manner.
 *
 * A severity is needed when creating a log entry. The severity only determines the string value for the severity column in the log entry.
 * However, log entries with a debug severity will only log if debug logging is enabled; otherwise, the log entry is elided at compile-time.
 *
 * In order to log objects, use the `LOG()` or `LOG_X()` macros followed by a sequence of insertion operators (`operator<<`). The object
 * following the insertion operator must have an overload for `operator<<` with an `std::ostream`.
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
     * @brief Initialize logging.
     *
     * @details Initialization must be done at least once before using any logging functionality.
     *
     * @param logFile The path to a file to log all log entries.
     *
     * @param truncate True if the log file should be truncated when opened; otherwise, the file will not be truncated when opened.
     */
    static void initialize(std::filesystem::path logFile, bool truncate);

    /**
     * @brief Determine if a log entry should be logged.
     *
     * @param severity The severity of the log entry.
     *
     * @return True if the log entry should be logged; otherwise, false.
     */
    [[nodiscard]] static constexpr bool shouldLog(Severity severity)
    {
        return severity == Severity::Debug ? LOG_DEBUG_SEVERITY : true;
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
     * @details The log entry should be filled with items using @c LogEntry::operator<<; otherwise, the log entry will only contain a header
     * with no information.
     *
     * @param severity The severity of the log entry.
     *
     * @param sourceFile The source file that the log entry is being created in. Usually, the value is `__FILE__`.
     *
     * @param line The line number of the source file that the log entry was created at. Usually, the value is `__LINE__`.
     *
     * @return A new log entry.
     */
    [[nodiscard]] static LogEntry createLogEntry(Severity severity, const std::filesystem::path& sourceFile, uint32_t line);

    /**
     * @brief Determines if debug severity will log a log entry. Usually, the value is determined by the project build mode.
     */
    static constexpr bool LOG_DEBUG_SEVERITY = true; //TODO Change value based off of project build mode (debug vs release)

private:
    //TODO Support file rotation. Once the current file passes some threshold (e.g. certain file size), create a new file to log to.
    /**
     * @brief Handles all logic for logging such as logging a @c LogEntry and file rotation.
     */
    class Logger
    {
    public:
        /**
         * @brief Construct a logger.
         *
         * @param logFile The path to a file to log all log entries.
         *
         * @param truncate True if the log file should be truncated when opened; otherwise, the file will not be truncated when opened.
         */
        explicit Logger(std::filesystem::path logFile, bool truncate);

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
         * @brief Log a log entry.
         *
         * @details This function is thread-safe. The reasoning behind using `operator+=` is to allow this function to be called after the
         * log entry has been fully built. Any operator could work as long as it is overloadable and does not take precedence over
         * `operator<<`, which is used for building the @c LogEntry.
         *
         * @param logEntry Log entry to be logged.
         */
        void operator+=(const LogEntry& logEntry);

    private:
        std::filesystem::path m_logFile;
        std::mutex m_mutex;
        std::fstream m_out;
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
         * @details This is the function to use when needing to log objects. Operator chaining is supported to allow easily insert multiple
         * objects.
         *
         * @tparam T Type of the object. This type must have an operator overload for `operator<<` with an `std::ostream`.
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
        std::string toString() const;

    private:
        std::stringstream m_builder;
    };

    inline static std::unique_ptr<Logger> s_logger;
};

#define LOG(severity) \
    if constexpr(!chat::common::Logging::shouldLog(severity)) {} \
    else chat::common::Logging::getLogger() += chat::common::Logging::createLogEntry(severity, __FILE__, __LINE__)
#define LOG_FATAL LOG(chat::common::Logging::Severity::Fatal)
#define LOG_ERROR LOG(chat::common::Logging::Severity::Error)
#define LOG_WARN LOG(chat::common::Logging::Severity::Warn)
#define LOG_INFO LOG(chat::common::Logging::Severity::Info)
#define LOG_DEBUG LOG(chat::common::Logging::Severity::Debug)

}
