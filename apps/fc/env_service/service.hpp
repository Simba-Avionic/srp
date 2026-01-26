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
  ara::core::Result<srp::apps::PressCalibrationRes> calPressureSensor() override {
    auto res = press_sensor->MakeCalibration(press_sensor_id);
    if (!res.has_value()) {
      return  ara::com::MakeErrorCode(
            ara::com::ComErrc::kWrongMethodCallProcessingMode, "Invalid request to Calibration PressureSensor ");
    }
    srp::apps::PressCalibrationRes someip_res;
    someip_res.a = res.value().a;
    someip_res.b = res.value().b;
    return someip_res;
  }
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_EC_ENV_SERVICE_SERVICE_HPP_
