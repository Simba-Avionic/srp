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
#include "mw/i2c_service/pca9685/controller/servoController.hpp"
namespace simba {
namespace router {

core::ErrorCode Router::Run(std::stop_token token) {
    while (true) {
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
      this->gpio_.Init(12);
      this->servo_.Init(12);
  return core::ErrorCode::kOk;
}
}  // namespace router
}  // namespace simba
