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
#ifndef APPS_EC_SERVOSERVICE_SERVO_SERVICE_DID_H_
#define APPS_EC_SERVOSERVICE_SERVO_SERVICE_DID_H_
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include "ara/log/log.h"
#include "ara/diag/generic_data_identifier.h"
#include "ara/diag/uds_error_domain.h"
namespace srp {
namespace service {

class ServoServiceDiD : public ara::diag::GenericDiD {
 private:
  std::shared_ptr<i2c::PCA9685>  servoController;
  const uint8_t actuator_id;
  ara::core::Result<ara::diag::OperationOutput> Read() noexcept override {
    auto res = this->servoController->ReadServoPosition(actuator_id);
    if (!res.has_value()) {
      return ara::diag::MakeErrorCode(
        ara::diag::UdsDiagErrc::kInvalidKey);
    }
    ara::log::LogInfo() << "ServoServiceDID.READ";
    return ara::diag::OperationOutput{{res.value()}};
  }
  ara::core::Result<void> Write(const std::vector<uint8_t>& payload) noexcept override {
    if (payload.size() != 1) {
      return ara::diag::MakeErrorCode(
        ara::diag::UdsDiagErrc::kInvalidMessageLengthFormat);
    }
    ara::log::LogInfo() <<  "Set position:" << payload[0] << ", to actuator ID:" <<this->actuator_id;
    auto res = this->servoController->AutoSetServoPosition(this->actuator_id, payload[0]);
    if (res != core::ErrorCode::kOk) {
      ara::log::LogWarn() << "huj";
      return ara::diag::MakeErrorCode(
        ara::diag::UdsDiagErrc::kFailurePreventsExecutionOfRequestedAction);
    }
    return {};
  }

 public:
  ServoServiceDiD(const ara::core::InstanceSpecifier& instance,
      std::shared_ptr<i2c::PCA9685> servo_controller, uint8_t actuator_id):
      servoController(servo_controller), actuator_id(actuator_id),
      ara::diag::GenericDiD{instance} {
      }
};

}  // namespace service
}  // namespace srp

#endif  // APPS_EC_SERVOSERVICE_SERVO_SERVICE_DID_H_
