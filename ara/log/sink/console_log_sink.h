/**
 * @file console_log_sink.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_LOG_SINK_CONSOLE_LOG_SINK_H_
#define ARA_LOG_SINK_CONSOLE_LOG_SINK_H_

#include <memory>

#include "ara/log/sink/log_sink.h"

namespace ara {
namespace log {
namespace sink {
class ConsoleLogSink : public LogSink {
 private:
 public:
  ConsoleLogSink();
  void Log(const LogStream &logStream) override;
};
}  // namespace sink
}  // namespace log
}  // namespace ara

#endif  // ARA_LOG_SINK_CONSOLE_LOG_SINK_H_
