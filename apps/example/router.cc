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
#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "diag/dtc/controller/dtc.h"
namespace simba {
namespace router {

core::ErrorCode Router::Run(std::stop_token token) {
  while (true) {
    AppLogger::Error("ZMIANA 1");
    this->servo_.AutoSetServoPosition(60, 1);
    this->servo_.AutoSetServoPosition(61, 1);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    AppLogger::Error("ZMIANA 2");
    this->servo_.AutoSetServoPosition(60, 0);
    this->servo_.AutoSetServoPosition(61, 0);
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode Router::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
      this->servo_.Init(this->app_id_, parms);
  return core::ErrorCode::kOk;
}

}  // namespace router
}  // namespace simba
