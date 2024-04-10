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

#include <memory>
#include <vector>

#include "communication-core/someip-controller/event_proxy.h"
#include "communication-core/someip-controller/method_skeleton.h"
#include "core/logger/Logger.h"
#include "diag/dtc/controller/dtc.h"
namespace simba {
namespace router {

core::ErrorCode Router::Run(std::stop_token token) {
  auto current_mode_proxy = std::make_shared<com::someip::EventProxyBase>(
      "ExampleApp/currentMode", [](const std::vector<uint8_t> payload) {
        AppLogger::Info("Current cpu mode: " + std::to_string(payload[0]));
      });
com->Add(current_mode_proxy);
current_mode_proxy->StartFindService();
  while (true) {
    this->SleepMainThread();
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
