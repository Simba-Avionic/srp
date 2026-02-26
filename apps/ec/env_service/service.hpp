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
#include "srp/env/EnvAppSkeleton.h"
#include "mw/i2c_service/controller/adcsensor/controller.hpp"

using EnvAppSkeleton = srp::env::EnvAppSkeleton;

namespace srp {
namespace apps {

class MyEnvAppSkeleton: public EnvAppSkeleton {
 private:
      std::shared_ptr<i2c::ADCSensorController> press_sensor;
      uint8_t press_sensor_id;

 public:
  MyEnvAppSkeleton(std::shared_ptr<i2c::ADCSensorController> press, const ara::core::InstanceSpecifier& instance,
                                                            const uint8_t press_id):
        press_sensor{press}, EnvAppSkeleton{instance}, press_sensor_id{press_id} {
  }
  ~MyEnvAppSkeleton() {
  }

 protected:
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_EC_ENV_SERVICE_SERVICE_HPP_
