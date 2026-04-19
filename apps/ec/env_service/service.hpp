/**
 * @file service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_EC_ENV_SERVICE_SERVICE_HPP_
#define APPS_EC_ENV_SERVICE_SERVICE_HPP_

#include <memory>
#include <algorithm>
#include "srp/env/EnvAppSkeleton.h"
#include "mw/i2c_service/controller/adcsensor/controller.hpp"

using EnvAppSkeleton = srp::env::EnvAppSkeleton;

namespace srp {
namespace apps {

class MyEnvAppSkeleton: public EnvAppSkeleton {
 private:
      uint16_t tank_value;

      uint16_t up_temp;
      uint16_t down_temp;

 public:
  explicit MyEnvAppSkeleton(const ara::core::InstanceSpecifier& instance): EnvAppSkeleton{instance} {
  }
  ~MyEnvAppSkeleton() {
  }
  void SetTankPressure(const uint16_t value) {
     this->tank_value = value;
  }
  void SetUpTankTemp(const uint16_t value) {
     this->up_temp = value;
  }
  void SetDownTankTemp(const uint16_t value) {
     this->down_temp = value;
  }
  ara::core::Result<std::uint16_t> GetTankPressure() override {
     return ara::core::Result<std::uint16_t>(this->tank_value);
  }
  ara::core::Result<std::uint16_t> GetUpperTankTemp() override {
     return ara::core::Result<std::uint16_t>(this->up_temp);
  }
  ara::core::Result<std::uint16_t> GetLowerTankTemp() override {
     return ara::core::Result<std::uint16_t>(this->down_temp);
  }

 protected:
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_EC_ENV_SERVICE_SERVICE_HPP_
