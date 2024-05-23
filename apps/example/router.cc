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

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>

#include <memory>
#include <vector>
#include "communication-core/someip-controller/event_proxy.h"
#include "communication-core/someip-controller/method_proxy.h"
#include "communication-core/someip-controller/method_skeleton.h"
#include "core/logger/Logger.h"
#include "diag/dtc/controller/dtc.h"
#include "mw/i2c_service/data/i2c_factory.h"
namespace simba {
namespace router {

core::ErrorCode Router::Run(std::stop_token token) {
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    this->pwm_.AutoSetServoPosition(61, 1);
    this->gpio_.SetPinValue(2, 1);
    auto res2 = this->gpio_.GetPinValue(2);
    if (res2.has_value()) {
      AppLogger::Warning("val:"+std::to_string(res2.value()));
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    this->pwm_.AutoSetServoPosition(61, 0);
    this->gpio_.SetPinValue(2, 0);
    auto res = this->gpio_.GetPinValue(2);
    if (res.has_value()) {
      AppLogger::Warning("val:"+std::to_string(res.value()));
    }
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode Router::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  this->gpio_ = gpio::GPIOController();
  this->pwm_.Init(parms);
  return core::ErrorCode::kOk;
}
}  // namespace router
}  // namespace simba
