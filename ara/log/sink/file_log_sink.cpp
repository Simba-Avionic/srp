/**
 * @file file_log_sink.cpp
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ara/log/sink/file_log_sink.h"

namespace ara {
namespace log {
namespace sink {
FileLogSink::FileLogSink(std::string logFilePath) : mLogFilePath{logFilePath} {}

void FileLogSink::Log(const LogStream &logStream) {
  //   const std::string cNewline{"\n"};

  //   LogStream _timestamp = GetTimestamp();
  //   LogStream _appstamp = GetAppstamp();
  //   _timestamp << cWhitespace << _appstamp << cWhitespace << logStream;
  //   std::string _logString = _timestamp.ToString();

  //   std::ofstream logFileStream(mLogFilePath,
  //                               std::ofstream::out | std::ofstream::app);
  //   logFileStream << _logString << cNewline;
  //   logFileStream.close();
}
}  // namespace sink
}  // namespace log
}  // namespace ara
