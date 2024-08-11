/**
 * @file console_log_sink.cpp
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/log/sink/console_log_sink.h"

#include <iostream>

namespace ara {
namespace log {
namespace sink {
ConsoleLogSink::ConsoleLogSink() {}

void ConsoleLogSink::Log(const LogStream &logStream) {
  LogStream out;
  out << this->GetTimestamp().ToString() << " [" << logStream.GetAppId()
      << "] [" << logStream.GetCtx() << "] [" << logStream.GetLogLevel()
      << "]: " << logStream.ToString();
  std::cout << out.ToString() << std::endl;
}
}  // namespace sink
}  // namespace log
}  // namespace ara
