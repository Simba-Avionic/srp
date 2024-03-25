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
#include "mw/i2c_service/servo/service/pca9685.hpp"
namespace simba {
namespace router {

core::ErrorCode Router::Run(std::stop_token token) {
    while (true) {
    this->pca_.SetServo(0, 4000);
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode Router::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
      this->gpio_.Init(12);
      if (this->i2c_.init() != core::ErrorCode::kOk) {
        AppLogger::Error("Nie udało sie zainicjować");
      }
      this->pca_.Init(12);
  return core::ErrorCode::kOk;
}
}  // namespace router
}  // namespace simba
