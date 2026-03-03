#ifndef CO_LOGGER_H
#define CO_LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <map>
#include <memory>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <mqueue.h>
#include "co_rfc5424.h"

/**
 * @class CoLogger
 * @brief Thread-safe RFC5424 compliant logger for ProviewR.
 *
 * Provides asynchronous logging with RFC5424 syslog format:
 * <PRI>VERSION TIMESTAMP HOSTNAME APP-NAME PROCID MSGID STRUCTURED-DATA MSG
 *
 * Supports two output modes:
 * 1. File logging: Logs to files in pwrp_log directory (or current directory if not set)
 * 2. POSIX Message Queue: When queue_name is provided, logs to POSIX message queue
 *
 * Structured Data Features:
 * - Uses "-" when no structured data is present (RFC5424 compliant)
 * - Supports flexible key-value pairs via addStructuredData()
 * - Default behavior replaces all data for an SD ID (predictable)
 * - Optional merge behavior when explicitly requested
 * - Removes hardcoded SSAB-specific defaults for worldwide compatibility
 *
 * Example outputs:
 * - No structured data: <134>1 2023-10-01T12:34:56.123+0200 myhost myapp 12345 - - INFO Hello World
 * - With structured data: <134>1 2023-10-01T12:34:56.123+0200 myhost myapp 12345 - [process@32473 pid="1234"
 * name="app"] INFO Hello World
 *
 * Usage examples:
 * @code
 * // Get logger instance
 * CoLogger& logger = CoLogger::instance("mymodule");
 *
 * // Simple logging
 * logger.log("Simple message");
 *
 * // Add structured data
 * logger.addStructuredData("process@32473", {{"pid", "1234"}, {"name", "myapp"}});
 * logger.log("Message with structured data");
 *
 * // Message queue usage
 * CoLogger& mq_logger = CoLogger::instance("module", "/myqueue");
 * mq_logger.log("Message to queue");
 * @endcode
 */
class CoLogger
{
public:
  /**
   * @brief Get a named logger instance (multiton pattern).
   * @return Reference to a logger for the given module name. Each module gets its own logger instance.
   * @param module_name Name of the module (used for log file naming and message identification).
   * @param queue_name Optional POSIX message queue name. If provided, logs to queue instead of file.
   * @return Reference to the logger instance for the module.
   */
  static CoLogger& instance(const std::string& module_name, const std::string& queue_name = "");

  /**
   * @brief Log a message asynchronously (thread-safe).
   *
   * Enqueues a log message to be written by the logging thread. Returns quickly, as no disk IO is done by
   * caller.
   * @param message The log message text.
   * @param level Log severity level (default: INFO).
   * @param facility Log facility (default: Local0).
   */
  void log(const std::string& message, CoLogLevel level = CoLogLevel::INFO,
           CoLogFacility facility = CoLogFacility::Local0);

  /**
   * @brief Set the minimum log level for this logger.
   *
   * Messages below this level will be ignored.
   * Thread-safe.
   * @param level The minimum log level to accept.
   */
  void setLogLevel(CoLogLevel level);

  /**
   * @brief Set the log facility for this logger.
   *
   * Thread-safe.
   * @param facility The facility to use for log messages.
   */
  void setFacility(CoLogFacility facility);

  /**
   * @brief Add a structured data element with key-value pairs using initializer list.
   *
   * Thread-safe. Adds structured data in RFC5424 format.
   * By default, replaces all key-value pairs for the given sd_id. Set merge=true to update/add keys.
   * Can be used for single or multiple pairs:
   * - Single: addStructuredData("id", {{"key", "value"}})
   * - Multiple: addStructuredData("id", {{"k1", "v1"}, {"k2", "v2"}})
   * - Replace (default): addStructuredData("id", {{"key", "new_value"}}) // Replaces all keys
   * - Merge: addStructuredData("id", {{"new_key", "value"}}, true) // Merges with existing keys
   * @param sd_id Structured Data ID (e.g., "exampleSDID@32473")
   * @param pairs Initializer list of key-value pairs
   * @param merge If true, merge with existing keys; if false (default), replace all keys for this sd_id
   */
  void addStructuredData(const std::string& sd_id,
                         std::initializer_list<std::pair<std::string, std::string>> pairs,
                         bool merge = false);

  /**
   * @brief Clear all structured data.
   *
   * Thread-safe. Resets structured data to empty state (will use "-" in logs).
   */
  void clearStructuredData();

  /**
   * @brief Check if structured data is present.
   *
   * Thread-safe.
   * @return true if structured data has been added, false otherwise
   */
  bool hasStructuredData() const;

  /**
   * @brief Destructor for CoLogger.
   *
   * Shuts down the logging thread and closes the log file.
   */
  ~CoLogger();

  /**
   * @brief Deleted copy constructor.
   */
  CoLogger(const CoLogger&) = delete;
  /**
   * @brief Deleted copy assignment operator.
   */
  CoLogger& operator=(const CoLogger&) = delete;
  /**
   * @brief Deleted move constructor.
   */
  CoLogger(CoLogger&&) = delete;
  /**
   * @brief Deleted move assignment operator.
   */
  CoLogger& operator=(CoLogger&&) = delete;

private:
  /**
   * @brief Constructor for CoLogger.
   *
   * Opens the log file or message queue and starts the logging thread.
   * @param module_name Name of the module (used for log file naming and message identification).
   * @param queue_name Optional POSIX message queue name. If empty, uses file logging.
   */
  explicit CoLogger(const std::string& module_name, const std::string& queue_name = "");
  std::ofstream m_logfile;
  mutable std::mutex m_mutex;
  std::string m_module_name;
  std::string m_queue_name;
  CoLogLevel m_log_level;
  CoLogFacility m_facility;
  std::map<std::string, std::map<std::string, std::string>> m_structured_data;
  mqd_t m_mqueue;
  bool m_use_mqueue;

  struct LogEntry
  {
    int64_t us_since_epoch;
    CoLogLevel level;
    CoLogFacility facility;
    std::string message;
    std::string module_name;
    std::map<std::string, std::map<std::string, std::string>> structured_data;
  };

  std::queue<LogEntry> m_mailbox;
  std::mutex m_mailbox_mutex;
  std::condition_variable m_mailbox_cv;
  std::thread m_logging_thread;
  std::atomic<bool> m_stop_thread{false};

  void loggingThreadFunc();
};

#endif // CO_LOGGER_H
