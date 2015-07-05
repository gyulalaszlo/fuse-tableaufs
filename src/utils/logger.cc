#include "logger.hpp"

#include <syslog.h>
#include <cstdarg>

using namespace tableauFS;

#define SYSLOG_VARIADIC_FN(name, level)                                        \
  void name(const char* fmt, ...)                                              \
  {                                                                            \
    va_list ap;                                                                \
    va_start(                                                                  \
        ap, fmt); /* Requires the last fixed parameter (to get the address) */ \
    vsyslog(level, fmt, ap);                                                   \
    va_end(ap);                                                                \
  }

namespace
{
  // Set the logging level of the cache in syslog
  enum LogLevel {
    Info = LOG_WARNING,
    Warn = LOG_WARNING,
    Error = LOG_ERR,
    Critical = LOG_CRIT,
  };
}

namespace tableauFS
{
  namespace log
  {
    SYSLOG_VARIADIC_FN(info, Info)
    SYSLOG_VARIADIC_FN(warn, Warn)
    SYSLOG_VARIADIC_FN(error, Error)
  }
}
