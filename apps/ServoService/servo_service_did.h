/**
 * @file example_did.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_SERVOSERVICE_SERVO_SERVICE_DID_H_
#define APPS_SERVOSERVICE_SERVO_SERVICE_DID_H_
#include <string>
#include <vector>
#include <strstream>
#include <memory>
#include "ara/log/log.h"
#include "ara/diag/generic_data_identifier.h"
namespace simba {
namespace service {

class ServoServiceDiD : public ara::diag::GenericDiD {
 private:
  std::shared_ptr<i2c::PCA9685>  servoController;
  const uint8_t actuator_id;
  ara::core::InstanceSpecifier instance_;
  ara::core::Result<ara::diag::OperationOutput> Read() noexcept override {
    auto res = this->servoController->ReadServoPosition(actuator_id);
    if (!res.has_value()) {
      return ara::diag::OperationOutput{};
    }
    ara::log::LogInfo() << "ServoServiceDID.READ";
    return ara::diag::OperationOutput{{res.value()}};
  }
  ara::core::Result<void> Write(const std::vector<uint8_t>& payload) noexcept override {
    if (payload.size() != 1) {
      return {};
    }
    ara::log::LogInfo() <<  "Set position:" << payload[0] << ", to actuator ID:" <<this->actuator_id;
    auto res = this->servoController->AutoSetServoPosition(this->actuator_id, payload[0]);
    if (res != core::ErrorCode::kOk) {
      ara::log::LogWarn() << "huj";
    }
    return {};
  }

 public:
  ServoServiceDiD(const ara::core::InstanceSpecifier& instance,
      std::shared_ptr<i2c::PCA9685> servo_controller, uint8_t actuator_id):
      servoController(servo_controller), actuator_id(actuator_id),
      instance_(instance), ara::diag::GenericDiD{instance_} {
      }
};

}  // namespace service
}  // namespace simba

#endif  // APPS_SERVOSERVICE_SERVO_SERVICE_DID_H_
