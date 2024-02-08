/**
 * @file logger_factory.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "core/logger/logger_factory.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "core/logger/console_logger.h"
namespace simba {
namespace core {
namespace logger {
namespace {
std::unordered_map<std::string, std::function<std::shared_ptr<ILogger>()>>
    lookup_table{
        {"kConsole", []() { return std::make_shared<ConsoleLogger>(); }}};
}  // namespace

Result<std::shared_ptr<ILogger>> LoggerFactory::CreateLogger(
    const std::string& name) {
  if (lookup_table.find(name) != lookup_table.end()) {
    auto obj = (lookup_table.at(name))();
    return Result{obj};
  } else {
    return Result<std::shared_ptr<ILogger>>{};
  }
}
}  // namespace logger
}  // namespace core
}  // namespace simba
