#include <chrono>
#include <ctime>
#include <iomanip>
#include <unistd.h>
#include <sys/types.h>

#include "co_logger.h"

CoLogger::CoLogger(const std::string& module_name)
    : m_module_name(module_name), m_log_level(CoLogLevel::INFO), m_facility(CoLogFacility::Local0),
      m_type("process"), m_subtype("proviewr")
{
  const char* logdir = std::getenv("pwrp_log");
  std::string filepath;
  if (logdir && logdir[0] != '\0')
  {
    filepath = std::string(logdir) + "/" + m_module_name + ".log";
  }
  else
  {
    filepath = m_module_name + ".log";
  }
  m_logfile.open(filepath, std::ios::app);

  m_stop_thread = false;
  m_logging_thread = std::thread(&CoLogger::loggingThreadFunc, this);
}

CoLogger& CoLogger::instance(const std::string& module_name)
{
  static std::map<std::string, std::unique_ptr<CoLogger>> loggers;
  auto it = loggers.find(module_name);
  if (it == loggers.end())
  {
    it = loggers.emplace(module_name, std::unique_ptr<CoLogger>(new CoLogger(module_name))).first;
  }
  return *it->second;
}

CoLogger::~CoLogger()
{
  // Signal thread to stop
  {
    std::lock_guard<std::mutex> lock(m_mailbox_mutex);
    m_stop_thread = true;
    m_mailbox_cv.notify_all();
  }
  if (m_logging_thread.joinable())
    m_logging_thread.join();

  if (m_logfile.is_open())
    m_logfile.close();
}

void CoLogger::log(const std::string& message, CoLogLevel level, CoLogFacility facility)
{
  if (static_cast<int>(level) > static_cast<int>(m_log_level))
    return;
  // Prepare log entry
  auto now = std::chrono::system_clock::now();
  LogEntry entry;
  entry.ms_since_epoch =
      std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
  entry.level = level;
  entry.facility = facility;
  entry.message = message;
  entry.module_name = m_module_name;
  entry.type = m_type;
  entry.subtype = m_subtype;
  entry.structured_prefix = m_structured_prefix;

  // Enqueue log entry
  {
    std::lock_guard<std::mutex> lock(m_mailbox_mutex);
    m_mailbox.push(std::move(entry));
    m_mailbox_cv.notify_one();
  }
}

void CoLogger::loggingThreadFunc()
{
  while (true)
  {
    std::unique_lock<std::mutex> lock(m_mailbox_mutex);
    m_mailbox_cv.wait(lock, [this] { return !m_mailbox.empty() || m_stop_thread; });

    if (m_stop_thread && m_mailbox.empty())
    {
      break;
    }

    while (!m_mailbox.empty())
    {
      // Extract log entry
      LogEntry entry = std::move(m_mailbox.front());
      m_mailbox.pop();

      // Release lock while doing IO so other threads can enqueue log messages
      lock.unlock();

      // Format and write log entry
      int used_facility = static_cast<int>(entry.facility);
      int pri = (used_facility * 8) + static_cast<int>(entry.level);

      // Construct time point
      std::chrono::system_clock::time_point tp(std::chrono::milliseconds(entry.ms_since_epoch));
      auto in_time_t = std::chrono::system_clock::to_time_t(tp);
      std::tm local_tm;
      localtime_r(&in_time_t, &local_tm);
      int ms = entry.ms_since_epoch % 1000;
      char tz_buf[8];
      strftime(tz_buf, sizeof(tz_buf), "%z", &local_tm);

      // Get hostname
      char hostname[128] = "localhost";
      gethostname(hostname, sizeof(hostname));

      // Map log levels to strings
      static const std::map<CoLogLevel, std::string> level_names = {
          {CoLogLevel::EMERGENCY, "EMERGENCY"}, {CoLogLevel::ALERT, "ALERT"},
          {CoLogLevel::CRITICAL, "CRITICAL"},   {CoLogLevel::ERROR, "ERROR"},
          {CoLogLevel::WARNING, "WARNING"},     {CoLogLevel::NOTICE, "NOTICE"},
          {CoLogLevel::INFO, "INFO"},           {CoLogLevel::DEBUG, "DEBUG"}};

      // Write log entry to file
      m_logfile << '<' << pri << '>' << std::put_time(&local_tm, "%Y-%m-%dT%H:%M:%S") << '.' << std::setw(3)
                << std::setfill('0') << ms << tz_buf << ' ' << hostname << ' ' << entry.module_name << ' '
                << getpid() << " - "
                << "[" << entry.structured_prefix << " log_type=\"" << entry.type << "\" log_subtype=\""
                << entry.subtype << "\"] " << level_names.at(entry.level) << " " << entry.message
                << std::endl;
      lock.lock();
    }
  }
}

// Set structured data prefix (customizable)
void CoLogger::setStructuredPrefix(const std::string& prefix)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_structured_prefix = prefix;
}

void CoLogger::setLogLevel(CoLogLevel level)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_log_level = level;
}

void CoLogger::setFacility(CoLogFacility facility)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_facility = facility;
}

void CoLogger::setType(const std::string& type)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_type = type;
}

void CoLogger::setSubtype(const std::string& subtype)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_subtype = subtype;
}
