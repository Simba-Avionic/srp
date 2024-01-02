/**
 * @file Logger.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "core/logger/Logger.h"

#include "core/logger/ILogger.h"

namespace simba {
namespace core {
namespace logger {
std::string Logger::appName = "NONE";  // NOLINT
loggingLevel Logger::level = loggingLevel::WARNING;
std::vector<std::shared_ptr<ILogger>> Logger::loggers = {};

void Logger::SetParms(const std::string& appName, loggingLevel lvl) {
  Logger::appName = appName;
  Logger::level = lvl;
}
void Logger::Debug(const std::string& log,
                   const std::source_location& location) {
  if (Logger::level <= DEBUG) {
    for (auto logger : Logger::loggers) {
      logger->Debug(Logger::GetLogMsg(log, location));
    }
  }
}
void Logger::Info(const std::string& log,
                  const std::source_location& location) {
  if (Logger::level <= INFO) {
    for (auto logger : Logger::loggers) {
      logger->Info(Logger::GetLogMsg(log, location));
    }
  }
}
void Logger::Warning(const std::string& log,
                     const std::source_location& location) {
  if (Logger::level <= WARNING) {
    for (auto logger : loggers) {
      logger->Warning(Logger::GetLogMsg(log, location));
    }
  }
}
void Logger::Error(const std::string& log,
                   const std::source_location& location) {
  if (Logger::level <= ERROR) {
    for (auto logger : loggers) {
      logger->Error(Logger::GetLogMsg(log, location));
    }
  }
}

void Logger::AddLogger(std::shared_ptr<ILogger> logger) {
  loggers.push_back(logger);
}
std::string Logger::GetLogMsg(const std::string& log,
                              const std::source_location& location) {
  const std::string file_path{location.file_name()};
  std::string file = file_path.substr(file_path.find_last_of("/") + 1);
  return file + ":" + std::to_string(location.line()) + "  " + log;
}
}  // namespace logger
}  // namespace core
}  // namespace simba
