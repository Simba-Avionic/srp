/**
 * @file console_logger.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file define console logger
 * @version 1.0
 * @date 2023-10-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "core/logger/console_logger.h"

#include <source_location>
#include <string>

#include "spdlog/spdlog.h"

namespace simba {
namespace core {
namespace logger {
ConsoleLogger::ConsoleLogger() { spdlog::set_level(spdlog::level::debug); }
simba::core::ErrorCode ConsoleLogger::Debug(const std::string& log) {
  spdlog::debug(log);
  return simba::core::ErrorCode::kOk;
}
simba::core::ErrorCode ConsoleLogger::Info(const std::string& log) {
  spdlog::info(log);
  return simba::core::ErrorCode::kOk;
}
simba::core::ErrorCode ConsoleLogger::Warning(const std::string& log) {
  spdlog::warn(log);
  return simba::core::ErrorCode::kOk;
}
simba::core::ErrorCode ConsoleLogger::Error(const std::string& log) {
  spdlog::error(log);
  return simba::core::ErrorCode::kOk;
}
}  // namespace logger
}  // namespace core
}  // namespace simba
