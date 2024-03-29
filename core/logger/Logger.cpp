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

#include <unordered_map>

#include "core/logger/ILogger.h"
namespace simba {
namespace core {
namespace logger {
namespace {
std::unordered_map<std::string, loggingLevel> lookup_table{
    {"kDebug", loggingLevel::DEBUG},
    {"kInfo", loggingLevel::INFO},
    {"kWarning", loggingLevel::WARNING},
    {"kError", loggingLevel::ERROR}};
}  // namespace

std::string Logger::appName = "NONE";  // NOLINT
loggingLevel Logger::level = loggingLevel::WARNING;
std::vector<std::shared_ptr<ILogger>> Logger::loggers = {};

loggingLevel Logger::ConvertStringToLogLevel(const std::string& val) {
  return lookup_table.at(val);
}

void Logger::SetParms(const std::string& appName, loggingLevel lvl) {
  Logger::appName = appName;
  Logger::level = lvl;
  for (auto logger : Logger::loggers) {
    logger->SetAppName(Logger::appName);
  }
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
  logger->SetAppName(Logger::appName);
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
