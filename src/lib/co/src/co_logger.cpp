#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <mqueue.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <map>

#include "co_logger.h"
#include "co_rfc5424.h"

CoLogger::CoLogger(const std::string& module_name, const std::string& queue_name)
    : m_module_name(module_name), m_queue_name(queue_name), m_log_level(CoLogLevel::INFO),
      m_facility(CoLogFacility::Local0), m_mqueue(-1), m_use_mqueue(false)
{
  std::cout << "CoLogger constructor called with module_name='" << module_name << "', queue_name='"
            << queue_name << "'" << std::endl;
  // Check if queue_name is provided to use POSIX message queue
  if (!queue_name.empty())
  {
    m_use_mqueue = true;

    // Append PWR_BUS_ID to queue name
    std::string full_queue_name = queue_name;
    const char* bus_id = std::getenv("PWR_BUS_ID");
    if (bus_id && bus_id[0] != '\0')
    {
      full_queue_name += "_";
      full_queue_name += bus_id;
    }

    // Set up message queue attributes
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 100;   // Maximum number of messages
    attr.mq_msgsize = 8192; // Maximum message size (8KB for log messages)
    attr.mq_curmsgs = 0;

    // Create and/or open message queue with bus_id appended
    m_mqueue = mq_open(full_queue_name.c_str(), O_CREAT | O_WRONLY, 0664, &attr);
    if (m_mqueue == (mqd_t)-1)
    {
      std::cerr << "mq_open failed for '" << full_queue_name << "': " << strerror(errno)
                << " (errno=" << errno << ")" << std::endl;

      // Fallback to file logging if mqueue fails
      m_use_mqueue = false;
      // Log error but continue with file logging
    }
    if (m_use_mqueue)
    {
      std::cout << "CoLogger using message queue: " << full_queue_name << std::endl;
    }
    else
    {
      std::cout << "CoLogger using file logging for module: " << m_module_name << std::endl;
    }
  }

  if (!m_use_mqueue)
  {
    // Traditional file logging
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
  }

  m_stop_thread = false;
  m_logging_thread = std::thread(&CoLogger::loggingThreadFunc, this);
}

CoLogger& CoLogger::instance(const std::string& module_name, const std::string& queue_name)
{
  static std::mutex instance_mutex;
  static std::map<std::string, std::unique_ptr<CoLogger>> loggers;

  std::lock_guard<std::mutex> lock(instance_mutex);

  // Create unique key combining module_name and queue_name
  std::string key = module_name + "|" + queue_name;

  auto it = loggers.find(key);
  if (it == loggers.end())
  {
    it = loggers.emplace(key, std::unique_ptr<CoLogger>(new CoLogger(module_name, queue_name))).first;
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

  if (m_use_mqueue && m_mqueue != (mqd_t)-1)
  {
    mq_close(m_mqueue);
  }
  else if (m_logfile.is_open())
  {
    m_logfile.close();
  }
}

void CoLogger::log(const std::string& message, CoLogLevel level, CoLogFacility facility)
{
  if (static_cast<int>(level) > static_cast<int>(m_log_level))
    return;
  // Prepare log entry
  auto now = std::chrono::system_clock::now();
  LogEntry entry;
  entry.us_since_epoch =
      std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
  entry.level = level;
  entry.facility = facility;
  entry.message = message;
  entry.module_name = m_module_name;
  entry.structured_data = m_structured_data;

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

      // Convert structured data to RFC5424 string format
      std::string structured_data_str = "-";
      if (!entry.structured_data.empty())
      {
        std::ostringstream sd_stream;
        for (const auto& sd_element : entry.structured_data)
        {
          sd_stream << "[" << sd_element.first;
          for (const auto& pair : sd_element.second)
          {
            sd_stream << " " << pair.first << "=\"" << pair.second << "\"";
          }
          sd_stream << "]";
        }
        structured_data_str = sd_stream.str();
      }

      // Use RFC5424 utilities to format header with entry timestamp
      std::string rfc5424_header = RFC5424::formatHeader(entry.level, entry.facility, entry.module_name,
                                                         structured_data_str, entry.us_since_epoch);

      // Map log levels to strings for the message content
      static const std::map<CoLogLevel, std::string> level_names = {
          {CoLogLevel::EMERGENCY, "EMERGENCY"}, {CoLogLevel::ALERT, "ALERT"},
          {CoLogLevel::CRITICAL, "CRITICAL"},   {CoLogLevel::ERROR, "ERROR"},
          {CoLogLevel::WARNING, "WARNING"},     {CoLogLevel::NOTICE, "NOTICE"},
          {CoLogLevel::INFO, "INFO"},           {CoLogLevel::DEBUG, "DEBUG"}};

      // Format complete RFC5424 log message
      std::ostringstream log_stream;
      log_stream << rfc5424_header << " " << level_names.at(entry.level) << " " << entry.message;

      std::string formatted_message = log_stream.str();

      if (m_use_mqueue && m_mqueue != (mqd_t)-1)
      {
        // Write to POSIX message queue
        if (mq_send(m_mqueue, formatted_message.c_str(), formatted_message.length(), 0) == -1)
        {
          std::cerr << "mq_send failed: " << strerror(errno) << " (errno=" << errno << ")" << std::endl;
        }
      }
      else
      {
        // Write to file
        m_logfile << formatted_message << std::endl;
      }
      lock.lock();
    }
  }
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

void CoLogger::addStructuredData(const std::string& sd_id,
                                 std::initializer_list<std::pair<std::string, std::string>> pairs, bool merge)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  if (merge)
  {
    // Merge behavior: Get or create the map for this SD ID, then update/merge the key-value pairs
    auto& sd_map = m_structured_data[sd_id];
    for (const auto& pair : pairs)
    {
      sd_map[pair.first] = pair.second; // This will update existing keys or add new ones
    }
  }
  else
  {
    // Replace behavior (default): Replace all key-value pairs for this SD ID
    std::map<std::string, std::string> pair_map;
    for (const auto& pair : pairs)
    {
      pair_map[pair.first] = pair.second;
    }
    m_structured_data[sd_id] = pair_map;
  }
}

void CoLogger::clearStructuredData()
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_structured_data.clear();
}

bool CoLogger::hasStructuredData() const
{
  std::lock_guard<std::mutex> lock(m_mutex);
  return !m_structured_data.empty();
}
