/*
 * C-compatible wrapper for RFC5424 formatting functionality.
 * This file isolates C++ dependencies to avoid compilation issues
 * when co_rfc5424_c.h is included in C code.
 */

#include "co_rfc5424_c.h"
#include "co_rfc5424.h"
#include <cstring>

// C-compatible wrapper function (no exceptions across C boundary)
extern "C" int co_rfc5424_format_header_c(char severity_char, const char* app_name, char* buffer,
                                          size_t buffer_size)
{
  if (!app_name || !buffer || buffer_size == 0)
  {
    return -1;
  }

  try
  {
    std::string header =
        RFC5424::formatHeaderCompat(severity_char, CoLogFacility::Local0, std::string(app_name));

    if (header.length() >= buffer_size)
    {
      return -1; // Buffer too small
    }

    std::strcpy(buffer, header.c_str());
    return static_cast<int>(header.length());
  }
  catch (...)
  {
    return -1; // Error occurred
  }
}