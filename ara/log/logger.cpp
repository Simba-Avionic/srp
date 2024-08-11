/**
 * @file logger.cpp
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ara/log/logger.h"

#include <memory>
namespace ara {
namespace log {

Logger::Logger(
    std::string ctxId, std::string ctxDescription, LogLevel ctxDefLogLevel,
    std::function<void(const Logger&, LogLevel, const std::string&)> callback)
    : mContextId{ctxId},
      mContextDescription{ctxDescription},
      mContextDefaultLogLevel{ctxDefLogLevel},
      callback_{callback} {}

void Logger::Log(const std::string& log, LogLevel level) const {
  callback_(*this, level, log);
}

ClientState Logger::RemoteClientState() const noexcept {
  // For now, no client exists for logging modeled messages.
  return ClientState::kNotConnected;
}

LogStream Logger::LogFatal() const {
  LogStream _result = WithLevel(LogLevel::kFatal);
  return _result;
}

LogStream Logger::LogError() const {
  LogStream _result = WithLevel(LogLevel::kError);
  return _result;
}

LogStream Logger::LogWarn() const {
  LogStream _result = WithLevel(LogLevel::kWarn);
  return _result;
}

LogStream Logger::LogInfo() const {
  LogStream _result = WithLevel(LogLevel::kInfo);
  return _result;
}

LogStream Logger::LogDebug() const {
  LogStream _result = WithLevel(LogLevel::kDebug);
  return _result;
}

LogStream Logger::LogVerbose() const {
  LogStream _result = WithLevel(LogLevel::kVerbose);
  return _result;
}

bool Logger::IsEnabled(LogLevel logLevel) const noexcept {
  // Log levels are sorted in descending order
  bool _result = (logLevel <= mContextDefaultLogLevel);
  return _result;
}

LogStream Logger::WithLevel(LogLevel logLevel) const {
  LogStream _result{logLevel, [this](const std::string& log, LogLevel level) {
                      this->Log(log, level);
                    }};

  return _result;
}

Logger Logger::CreateLogger(
    std::string ctxId, std::string ctxDescription, LogLevel ctxDefLogLevel,
    std::function<void(const Logger&, LogLevel, const std::string&)> callback) {
  Logger _result(ctxId, ctxDescription, ctxDefLogLevel, callback);
  return _result;
}

}  // namespace log
}  // namespace ara
