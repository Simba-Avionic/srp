/**
 * @file logger_factory.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef CORE_LOGGER_LOGGER_FACTORY_H_
#define CORE_LOGGER_LOGGER_FACTORY_H_
#include <memory>
#include <string>
#include <optional>

#include "core/logger/ILogger.h"

namespace simba {
namespace core {
namespace logger {
class LoggerFactory {
 public:
  static std::optional<std::shared_ptr<ILogger>> CreateLogger(const std::string& name);
};
}  // namespace logger
}  // namespace core
}  // namespace simba
#endif  // CORE_LOGGER_LOGGER_FACTORY_H_
