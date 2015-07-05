#pragma once

namespace tableauFS
{
  // Generic logging to syslog
  namespace log
  {
    void info(const char* fmt, ...);
    void warn(const char* fmt, ...);
    void error(const char* fmt, ...);
  }
}
