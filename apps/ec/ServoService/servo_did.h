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
#ifndef APPS_EC_SERVOSERVICE_SERVO_DID_H_
#define APPS_EC_SERVOSERVICE_SERVO_DID_H_
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include "ara/log/log.h"
#include "ara/diag/generic_data_identifier.h"
#include "apps/ec/ServoService/servoController/servo_controller.hpp"
#include "ara/com/com_error_domain.h"
#include "ara/diag/uds_error_domain.h"
#include "core/common/error_code.h"
namespace srp {
namespace service {


class ServoSecondDid : public ara::diag::GenericDiD {
 private:
  std::shared_ptr<ServoController>  servoController;
  ara::core::Result<ara::diag::OperationOutput> Read() noexcept override {
    return ara::diag::MakeErrorCode(
      ara::diag::UdsDiagErrc::kSubFunctionNotSupported);
  }
  ara::core::Result<void> Write(
      const std::vector<uint8_t> &payload) noexcept override {
        ara::log::LogWarn() << "receive write";
    if (servoController == nullptr) {
      return ara::diag::MakeErrorCode(
        ara::diag::UdsDiagErrc::kProgrammingFailure);
    }
    if (payload.size() != 5) {
        return ara::diag::MakeErrorCode(
          ara::diag::UdsDiagErrc::kInvalidMessageLengthFormat);
    }
    uint16_t open_pos = payload[1] << 8 | payload[2];
    uint16_t close_pos = payload[3] << 8 | payload[4];
    if (!servoController->ChangeConfigPosition(payload[0], open_pos, close_pos)) {
        return ara::diag::MakeErrorCode(
          ara::diag::UdsDiagErrc::kFailurePreventsExecutionOfRequestedAction);
    }
    return {};
  }

 public:
  ServoSecondDid(const ara::core::InstanceSpecifier &specifier,
      std::shared_ptr<ServoController> servo_controller):
      ara::diag::GenericDiD{specifier},
      servoController(servo_controller) {
    }
  ~ServoSecondDid() = default;
};

}  // namespace service
}  // namespace srp

#endif  // APPS_EC_SERVOSERVICE_SERVO_DID_H_
