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
#include "mw/i2c_service/pca9685/controller/servoController.hpp"
namespace simba {
namespace router {

core::ErrorCode Router::Run(std::stop_token token) {
  while (true) {
    auto proxy_event = std::make_shared<com::someip::EventProxyBase>(
        "ExampleApp/someevent",
        [this](const std::vector<uint8_t>) { AppLogger::Info("EVENT !!!!!!!"); });
    auto example = std::make_shared<com::someip::MethodSkeleton>(
        "ExampleApp/exampleMethod",
        [this](const std::vector<uint8_t> payload)
            -> std::optional<std::vector<uint8_t>> {
          return std::vector<uint8_t>{0, 1, 2};
        });
    com->Add(example);
    com->Add(proxy_event);
    proxy_event->StartFindService();
    this->servo_.SetServoPos(60, 0);
    this->servo_.SetServoPos(61, 0);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    this->servo_.SetServoPos(60, 1);
    this->servo_.SetServoPos(61, 1);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode Router::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
      this->gpio_ = gpio::GPIOController(new com::soc::IpcSocket());
      this->gpio_.Init(this->app_id_);
      this->servo_.Init(12);
  return core::ErrorCode::kOk;
}
}  // namespace router
}  // namespace simba
