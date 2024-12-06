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
#include <strstream>
#include <memory>
#include "ara/log/log.h"
#include "diag/jobs/skeleton/did_job.h"
namespace simba {
namespace service {

class ServoServiceDiD : public diag::DiDJob {
 private:
  const uint8_t actuator_id;
  std::shared_ptr<i2c::PCA9685>  servoController;
  /**
   * @brief Callback for 0x22 UDS job (Read data by id) *optional
   *
   * @return DiagResponse
   */
  diag::DiagResponse Read() {
    auto res = this->servoController->ReadServoPosition(actuator_id);
    if (!res.has_value()) {
      return diag::DiagResponse(diag::DiagResponseCodes::kConditionsNotCorrect);
    }
    ara::log::LogInfo() << "ServoServiceDID.READ" << instance_.ToString() << "::" << res.value();
    return diag::DiagResponse(diag::DiagResponseCodes::kOk,
                              {res.value()});
  }
diag::DiagResponse Write(const std::vector<uint8_t>& payload) {
  if (payload.size() != 1) {
    return diag::DiagResponse{diag::DiagResponseCodes::kInvalidMessageLengthFormat};
  }
  ara::log::LogInfo() <<  "Set position:" << payload[0] << ", to actuator ID:" <<this->actuator_id;
  this->servoController->AutoSetServoPosition(this->actuator_id, payload[0]);
  return diag::DiagResponse{diag::DiagResponseCodes::kOk};
}

 public:
  ServoServiceDiD(const ara::core::InstanceSpecifier& instance,
      std::shared_ptr<i2c::PCA9685> servo_controller, uint8_t actuator_id):
      diag::DiDJob(instance), servoController(servo_controller), actuator_id(actuator_id) {
      }
};

}  // namespace service
}  // namespace simba

#endif  // APPS_EC_SERVOSERVICE_SERVO_SERVICE_DID_H_
