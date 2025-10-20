#ifndef CO_RFC5424_H
#define CO_RFC5424_H

#include <string>
#include <chrono>

/**
 * @brief RFC5424 protocol version constant.
 *
 * RFC5424 specifies that the VERSION field should be "1" for compliant messages.
 */
constexpr int RFC5424_VERSION = 1;

/**
 * @enum CoLogLevel
 * @brief RFC5424 syslog severity levels.
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
 * @brief RFC5424 syslog facility codes.
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
 * @namespace RFC5424
 * @brief RFC5424 syslog formatting utilities.
 *
 * Provides common RFC5424 syslog header formatting functionality
 * for use by co_logger, errh, and other ProviewR components.
 */
namespace RFC5424
{

/**
 * @brief Format RFC5424 header with current timestamp.
 *
 * Generates RFC5424 compliant syslog header with current timestamp:
 * <PRI>VERSION TIMESTAMP HOSTNAME APP-NAME PROCID MSGID STRUCTURED-DATA
 *
 * @param level Log severity level
 * @param facility Log facility
 * @param app_name Application/module name
 * @param structured_data Optional structured data string (default: "-")
 * @return RFC5424 formatted header string
 */
std::string formatHeader(CoLogLevel level, CoLogFacility facility, const std::string& app_name,
                         const std::string& structured_data = "-");

/**
 * @brief Format RFC5424 header with specific timestamp.
 *
 * Generates RFC5424 compliant syslog header with provided timestamp:
 * <PRI>VERSION TIMESTAMP HOSTNAME APP-NAME PROCID MSGID STRUCTURED-DATA
 *
 * @param level Log severity level
 * @param facility Log facility
 * @param app_name Application/module name
 * @param structured_data Optional structured data string (default: "-")
 * @param us_since_epoch Microseconds since Unix epoch
 * @return RFC5424 formatted header string
 */
std::string formatHeader(CoLogLevel level, CoLogFacility facility, const std::string& app_name,
                         const std::string& structured_data, int64_t us_since_epoch);

/**
 * @brief Format RFC5424 header with current timestamp (character-based severity).
 *
 * Compatibility function for errh module that uses character-based severity levels.
 * Maps severity characters to RFC5424 numeric levels.
 *
 * @param severity_char Character representing severity ('E', 'W', 'I', etc.)
 * @param facility Log facility
 * @param app_name Application/module name
 * @param structured_data Optional structured data string (default: "-")
 * @return RFC5424 formatted header string
 */
std::string formatHeaderCompat(char severity_char, CoLogFacility facility, const std::string& app_name,
                               const std::string& structured_data = "-");

/**
 * @brief Format timezone string from +0200 to +02:00 format.
 *
 * Converts timezone from strftime %z format to RFC5424 compliant format.
 * @param tz_string Timezone string from strftime (e.g., "+0200")
 * @return RFC5424 formatted timezone string (e.g., "+02:00")
 */
std::string formatTimezone(const std::string& tz_string);

/**
 * @brief Map character-based severity to CoLogLevel enum.
 *
 * Maps legacy character-based severity levels to RFC5424 numeric levels.
 * @param severity_char Character representing severity ('E', 'W', 'I', etc.)
 * @return Corresponding CoLogLevel enum value
 */
CoLogLevel mapSeverityChar(char severity_char);

} // namespace RFC5424

#endif // CO_RFC5424_H