/**
 * @file console_logger.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file define console logger
 * @version 1.0
 * @date 2023-10-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef CORE_LOGGER_CONSOLE_LOGGER_H_
#define CORE_LOGGER_CONSOLE_LOGGER_H_
#include <string>

#include "core/common/error_code.h"
#include "core/logger/ILogger.h"
namespace simba {
namespace core {
namespace logger {

class ConsoleLogger : public ILogger {
 public:
  ConsoleLogger();
  simba::core::ErrorCode Debug(const std::string& log) override;
  simba::core::ErrorCode Info(const std::string& log) override;
  simba::core::ErrorCode Warning(const std::string& log) override;
  simba::core::ErrorCode Error(const std::string& log) override;
};
}  // namespace logger
}  // namespace core
}  // namespace simba
#endif  // CORE_LOGGER_CONSOLE_LOGGER_H_
