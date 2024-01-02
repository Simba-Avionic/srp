/**
 * @file ILogger.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file define interface for logger
 * @version 0.1
 * @date 2023-10-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef CORE_LOGGER_ILOGGER_H_
#define CORE_LOGGER_ILOGGER_H_
#include <source_location>
#include <string>

#include "core/common/error_code.h"
namespace simba {
namespace core {
namespace logger {

enum loggingLevel { DEBUG, INFO, WARNING, ERROR, OFF };

class ILogger {
 public:
  virtual simba::core::ErrorCode Debug(const std::string& log) = 0;
  virtual simba::core::ErrorCode Info(const std::string& log) = 0;
  virtual simba::core::ErrorCode Warning(const std::string& log) = 0;
  virtual simba::core::ErrorCode Error(const std::string& log) = 0;
};
}  // namespace logger
}  // namespace core
}  // namespace simba
#endif  // CORE_LOGGER_ILOGGER_H_
