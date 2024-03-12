/**
 * @file router.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "apps/example/router.h"

#include "core/logger/Logger.h"
namespace simba {
namespace router {

core::ErrorCode Router::Run(std::stop_token token) {
  while (true) {
    AppLogger::Debug("AppLogger::Debug");
    AppLogger::Info("AppLogger::Info");
    AppLogger::Warning("AppLogger::Warning");
    this->gpio_.SetPinValue(1, gpio::Value::HIGH);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    this->gpio_.SetPinValue(1, gpio::Value::LOW);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode Router::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
      this->gpio_ = gpio::GPIOController(new com::soc::IpcSocket());
      this->gpio_.Init(12);
  return core::ErrorCode::kOk;
}
}  // namespace router
}  // namespace simba
