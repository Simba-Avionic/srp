/**
 * @file logging_menager.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_LOG_LOGGING_MENAGER_H_
#define ARA_LOG_LOGGING_MENAGER_H_

#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "ara/log/logger.h"
#include "ara/log/sink/log_sink.h"

namespace ara {
namespace log {

class LoggingMenager {
 private:
  const std::string appId_;
  LogLevel mDefaultLogLevel;
  std::unordered_map<std::string, Logger> mLoggers;
  std::list<std::unique_ptr<sink::LogSink>> sink_list_{};
  static std::unique_ptr<LoggingMenager> loger_f_;

 public:
  LoggingMenager(const std::string &appId, LogLevel logLevel);
  LoggingMenager() = delete;
  ~LoggingMenager() noexcept;
  const Logger &CreateLogger(std::string ctxId);

  const Logger &CreateLogger(std::string ctxId, std::string ctxDescription);

  const Logger &CreateLogger(std::string ctxId, std::string ctxDescription,
                             LogLevel ctxDefLogLevel);

  void Log(const Logger &logger, LogLevel logLevel,
           const std::string &logStream);

  static void Create(std::string appId, LogMode logMode,
                     LogLevel logLevel = LogLevel::kWarn,
                     std::string appDescription = "");
  static void AddSink(std::unique_ptr<sink::LogSink> sink_);
  static void Clear();
  static LoggingMenager *GetInstance();
};
}  // namespace log
}  // namespace ara

#endif  // ARA_LOG_LOGGING_MENAGER_H_
