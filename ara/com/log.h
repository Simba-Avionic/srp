/**
 * @file log.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_COM_LOG_H_
#define ARA_COM_LOG_H_
#include "ara/log/logging_menager.h"

namespace ara {
namespace com {

static const ara::log::Logger& CreateComLoggerInstance() {
  const auto& logger = ara::log::LoggingMenager::GetInstance()->CreateLogger(
      "acom", "Default ctx for ara::com", ara::log::LogLevel::kVerbose);
  return logger;
}
/// @brief Create a stream for fatal logs
/// @returns Fatal log stream in the current context
/// @see WithLevel
static ara::log::LogStream __attribute__((unused)) LogFatal() {
  return CreateComLoggerInstance().LogFatal();
}

/// @brief Create a stream for error logs
/// @returns Error log stream in the current context
/// @see WithLevel
static ara::log::LogStream __attribute__((unused)) LogError() {
  return CreateComLoggerInstance().LogError();
}

/// @brief Create a stream for warning logs
/// @returns Warning log stream in the current context
/// @see WithLevel
static ara::log::LogStream __attribute__((unused)) LogWarn() {
  return CreateComLoggerInstance().LogWarn();
}

/// @brief Create a stream for information logs
/// @returns Information log stream in the current context
/// @see WithLevel
static ara::log::LogStream __attribute__((unused)) LogInfo() {
  return CreateComLoggerInstance().LogInfo();
}

/// @brief Create a stream for debug logs
/// @returns Debug log stream in the current context
/// @see WithLevel
static ara::log::LogStream __attribute__((unused)) LogDebug() {
  return CreateComLoggerInstance().LogDebug();
}

/// @brief Create a stream for verbose logs
/// @returns Verbose log stream in the current context
/// @see WithLevel
static ara::log::LogStream __attribute__((unused)) LogVerbose() {
  return CreateComLoggerInstance().LogVerbose();
}
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_LOG_H_
