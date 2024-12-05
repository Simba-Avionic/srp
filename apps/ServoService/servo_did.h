/**
 * @file servo_did.h
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_SERVOSERVICE_SERVO_DID_H_
#define APPS_SERVOSERVICE_SERVO_DID_H_
#include <string>
#include <vector>
#include <strstream>
#include <memory>
#include "ara/log/log.h"
#include "ara/diag/generic_data_identifier.h"
#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "ara/com/com_error_domain.h"
namespace simba {
namespace service {


class ServoSecondDid : public ara::diag::GenericDiD {
 private:
  ara::core::InstanceSpecifier specifier_;
  std::shared_ptr<i2c::PCA9685>  servoController;
  ara::core::Result<ara::diag::OperationOutput> Read() noexcept override {
    if (servoController == nullptr) {
      return ara::diag::OperationOutput{};
    }
    ara::log::LogWarn() << "receive read";
    return ara::diag::OperationOutput{{1}};
  }
  ara::core::Result<void> Write(
      const std::vector<uint8_t> &payload) noexcept override {
        ara::log::LogWarn() << "receive write";
    if (servoController == nullptr) {
      return {};
    }
    if (payload.size() != 5) {
        return {};
    }
    uint16_t open_pos = payload[1] << 8 | payload[2];
    uint16_t close_pos = payload[3] << 8 | payload[4];
    if (!servoController->ChangeConfigPosition(payload[0], open_pos, close_pos)) {
        return {};
    }
    return {};
  }

 public:
  ServoSecondDid(const ara::core::InstanceSpecifier &specifier,
      std::shared_ptr<i2c::PCA9685> servo_controller): specifier_(specifier),
    servoController(servo_controller), ara::diag::GenericDiD{specifier_} {

    }
  ~ServoSecondDid() = default;
};

}  // namespace service
}  // namespace simba

#endif  // APPS_SERVOSERVICE_SERVO_DID_H_
