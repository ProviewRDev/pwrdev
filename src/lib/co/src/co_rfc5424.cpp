#include "co_rfc5424.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <unistd.h>

namespace RFC5424
{

std::string formatTimezone(const std::string& tz_string)
{
  // Convert timezone format from +0200 to +02:00
  if (tz_string.length() == 5 && (tz_string[0] == '+' || tz_string[0] == '-'))
  {
    return tz_string.substr(0, 3) + ":" + tz_string.substr(3);
  }
  return tz_string; // Return as-is if format is unexpected
}

CoLogLevel mapSeverityChar(char severity_char)
{
  switch (severity_char)
  {
  case 'E':
  case 'e':
    return CoLogLevel::ERROR;
  case 'W':
  case 'w':
    return CoLogLevel::WARNING;
  case 'I':
  case 'i':
    return CoLogLevel::INFO;
  case 'S':
  case 's':
    return CoLogLevel::NOTICE; // Success as Notice
  case 'F':
  case 'f':
    return CoLogLevel::CRITICAL; // Fatal as Critical
  case 'D':
  case 'd':
    return CoLogLevel::DEBUG;
  default:
    return CoLogLevel::INFO;
  }
}

std::string formatHeader(CoLogLevel level, CoLogFacility facility, const std::string& app_name,
                         const std::string& structured_data)
{
  // Calculate PRI value: facility * 8 + severity
  int pri = static_cast<int>(facility) * 8 + static_cast<int>(level);

  // Get current timestamp with microsecond precision
  auto now = std::chrono::system_clock::now();
  auto time_t_now = std::chrono::system_clock::to_time_t(now);
  auto us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000000;

  // Format timestamp with timezone
  struct tm local_tm;
  localtime_r(&time_t_now, &local_tm);

  char tz_buf[16];
  strftime(tz_buf, sizeof(tz_buf), "%z", &local_tm);
  std::string tz_formatted = formatTimezone(std::string(tz_buf));

  // Get hostname
  char hostname[128] = "localhost";
  gethostname(hostname, sizeof(hostname));

  // Build RFC5424 header
  std::ostringstream header_stream;
  header_stream << '<' << pri << '>' << RFC5424_VERSION << ' '
                << std::put_time(&local_tm, "%Y-%m-%dT%H:%M:%S") << '.' << std::setw(6) << std::setfill('0')
                << us.count() << tz_formatted << ' ' << hostname << ' ' << app_name << ' ' << getpid()
                << " - " << structured_data;

  return header_stream.str();
}

std::string formatHeader(CoLogLevel level, CoLogFacility facility, const std::string& app_name,
                         const std::string& structured_data, int64_t us_since_epoch)
{
  // Calculate PRI value: facility * 8 + severity
  int pri = static_cast<int>(facility) * 8 + static_cast<int>(level);

  // Construct time point from microseconds since epoch
  std::chrono::system_clock::time_point tp{std::chrono::microseconds(us_since_epoch)};
  auto time_t_now = std::chrono::system_clock::to_time_t(tp);
  int us = us_since_epoch % 1000000;

  // Format timestamp with timezone
  struct tm local_tm;
  localtime_r(&time_t_now, &local_tm);

  char tz_buf[16];
  strftime(tz_buf, sizeof(tz_buf), "%z", &local_tm);
  std::string tz_formatted = formatTimezone(std::string(tz_buf));

  // Get hostname
  char hostname[128] = "localhost";
  gethostname(hostname, sizeof(hostname));

  // Build RFC5424 header
  std::ostringstream header_stream;
  header_stream << '<' << pri << '>' << RFC5424_VERSION << ' '
                << std::put_time(&local_tm, "%Y-%m-%dT%H:%M:%S") << '.' << std::setw(6) << std::setfill('0')
                << us << tz_formatted << ' ' << hostname << ' ' << app_name << ' ' << getpid() << " - "
                << structured_data;

  return header_stream.str();
}

std::string formatHeaderCompat(char severity_char, CoLogFacility facility, const std::string& app_name,
                               const std::string& structured_data)
{
  // Map character-based severity to CoLogLevel
  CoLogLevel level = mapSeverityChar(severity_char);

  // Use the main header formatting function
  return formatHeader(level, facility, app_name, structured_data);
}

} // namespace RFC5424