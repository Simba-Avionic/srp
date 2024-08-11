/**
 * @file logger.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_LOG_LOGGER_H_
#define ARA_LOG_LOGGER_H_

#include <functional>
#include <memory>
#include <string>

#include "ara/log/log_stream.h"

namespace ara {
namespace log {
class LoggingFramework;
/// @brief Logger of a specific context
class Logger {
 private:
  std::string mContextId;
  std::string mContextDescription;
  LogLevel mContextDefaultLogLevel;
  LoggingFramework const* parent_;
  std::function<void(const Logger&, LogLevel, const std::string&)> callback_;
  Logger(std::string ctxId, std::string ctxDescription, LogLevel ctxDefLogLevel,
         std::function<void(const Logger&, LogLevel, const std::string&)>
             callback);

 public:
  std::string GetContext() const { return mContextId; }
  Logger() = delete;
  ~Logger() noexcept = default;
  void Log(const std::string& log, LogLevel level) const;
  /// @brief Remote logging client connection state
  /// @returns Client connection state
  ClientState RemoteClientState() const noexcept;

  /// @brief Create a stream for fatal logs
  /// @returns Fatal log stream in the current context
  /// @see WithLevel
  LogStream LogFatal() const;

  /// @brief Create a stream for error logs
  /// @returns Error log stream in the current context
  /// @see WithLevel
  LogStream LogError() const;

  /// @brief Create a stream for warning logs
  /// @returns Warning log stream in the current context
  /// @see WithLevel
  LogStream LogWarn() const;

  /// @brief Create a stream for information logs
  /// @returns Information log stream in the current context
  /// @see WithLevel
  LogStream LogInfo() const;

  /// @brief Create a stream for debug logs
  /// @returns Debug log stream in the current context
  /// @see WithLevel
  LogStream LogDebug() const;

  /// @brief Create a stream for verbose logs
  /// @returns Verbose log stream in the current context
  /// @see WithLevel
  LogStream LogVerbose() const;

  /// @brief Determine whether a certian log level is enabled in the current
  /// context or not
  /// @param logLevel Input log severity level
  /// @returns True if the level is enabled; otherwise false
  bool IsEnabled(LogLevel logLevel) const noexcept;

  /// @brief Create a stream for certian level logs
  /// @param logLevel Input log severity level
  /// @returns Log stream with the determined level in the current context
  LogStream WithLevel(LogLevel logLevel) const;

  /// @brief Logger factory
  /// @param ctxId Context ID
  /// @param ctxDescription Context description
  /// @param ctxDefLogLevel Context default log level
  /// @returns A new logger for that specifc context
  /// @note Log with less severity than the default log level are ignored.
  static Logger CreateLogger(
      std::string ctxId, std::string ctxDescription, LogLevel ctxDefLogLevel,
      std::function<void(const Logger&, LogLevel, const std::string&)>
          callback);
};

}  // namespace log
}  // namespace ara

#endif  // ARA_LOG_LOGGER_H_
