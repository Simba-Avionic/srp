/**
 * @file log_sink.cpp
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ara/log/sink/log_sink.h"

#include <time.h>

namespace ara {
namespace log {
namespace sink {

LogStream LogSink::GetAppstamp() const {
  LogStream _result;
  return _result;
}

uint32_t LogSink::GetTimeStamp() const noexcept {
  timespec t;
  clock_gettime(CLOCK_BOOTTIME, &t);
  return static_cast<uint32_t>(t.tv_sec * 10000 + t.tv_nsec / 100000L);
}

LogStream LogSink::GetTimestamp() const {
  timespec t;
  clock_gettime(CLOCK_BOOTTIME, &t);
  LogStream _result;
  _result << static_cast<uint32_t>(t.tv_sec) << "."
          << static_cast<uint32_t>(t.tv_nsec / 100000L);

  return _result;
}
}  // namespace sink
}  // namespace log
}  // namespace ara
