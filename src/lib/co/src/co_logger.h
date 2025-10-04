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

/**
 * @enum CoLogLevel
 * @brief Log severity levels for CoLogger.
 *
 * Used to specify the importance of log messages.
 */
enum class CoLogLevel : int
{
  EMERGENCY = 0, ///< System unusable
  ALERT = 1,     ///< Immediate action required
  CRITICAL = 2,  ///< Critical conditions
  ERROR = 3,     ///< Error conditions
  WARNING = 4,   ///< Warning conditions
  NOTICE = 5,    ///< Normal but significant condition
  INFO = 6,      ///< Informational messages
  DEBUG = 7      ///< Debug-level messages
};

/**
 * @enum CoLogFacility
 * @brief Log facility codes for CoLogger.
 *
 * Used to categorize the source of log messages.
 */
enum class CoLogFacility : int
{
  Local0 = 16, ///< Custom applications
  Local1 = 17, ///< Message queue systems
  Local2 = 18, ///< Database systems
  Local3 = 19, ///< Authentication services
  Local4 = 20, ///< Monitoring systems
  Local5 = 21, ///< Security systems
  Local6 = 22, ///< Network services
  Local7 = 23  ///< Development/testing/debugging
};

/**
 * @brief RFC5424 protocol version constant.
 *
 * RFC5424 specifies that the VERSION field should be "1" for compliant messages.
 */
constexpr int RFC5424_VERSION = 1;

/**
 * @class CoLogger
 * @brief Thread-safe RFC5424 compliant logger for ProviewR.
 *
 * Provides asynchronous logging with RFC5424 syslog format:
 * <PRI>VERSION TIMESTAMP HOSTNAME APP-NAME PROCID MSGID STRUCTURED-DATA MSG
 *
 * Supports two output modes:
 * 1. File logging: When module_name doesn't start with '/', logs to files in pwrp_log directory
 * 2. POSIX Message Queue: When module_name starts with '/', logs to POSIX message queue
 *
 * Example output:
 * <134>1 2023-10-01T12:34:56.123+0200 myhost myapp 12345 - [type="process" subtype="proviewr"] INFO Hello
 * World
 *
 * Message Queue Usage:
 * CoLogger::instance("module", "queue").log("Test message");
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
   * @brief Set the log type string for structured data.
   *
   * Thread-safe.
   * @param type The log type string.
   */
  void setType(const std::string& type);

  /**
   * @brief Set the log subtype string for structured data.
   *
   * Thread-safe.
   * @param subtype The log subtype string.
   */
  void setSubtype(const std::string& subtype);

  /**
   * @brief Set the structured data prefix for log messages.
   *
   * Thread-safe.
   * @param prefix The structured data prefix string.
   */
  void setStructuredPrefix(const std::string& prefix);

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
  std::mutex m_mutex;
  std::string m_module_name;
  std::string m_queue_name;
  CoLogLevel m_log_level;
  CoLogFacility m_facility;
  std::string m_type;
  std::string m_subtype;
  std::string m_structured_prefix =
      "ot-standard"; // SSAB Oxelösund Operational Technology standard, can be changed
  mqd_t m_mqueue;
  bool m_use_mqueue;

  struct LogEntry
  {
    int64_t ms_since_epoch;
    CoLogLevel level;
    CoLogFacility facility;
    std::string message;
    std::string module_name;
    std::string type;
    std::string subtype;
    std::string structured_prefix;
  };

  std::queue<LogEntry> m_mailbox;
  std::mutex m_mailbox_mutex;
  std::condition_variable m_mailbox_cv;
  std::thread m_logging_thread;
  std::atomic<bool> m_stop_thread{false};

  void loggingThreadFunc();
};

#endif // CO_LOGGER_H
