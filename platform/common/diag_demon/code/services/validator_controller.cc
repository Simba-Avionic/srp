/**
 * @file validator_controller.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-04-28
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "platform/common/diag_demon/code/services/validator_controller.h"

namespace srp {
namespace platform {
namespace diag_demon {
namespace uds {

UdsNrcCodes ValidatorController::Validate(const UdsRequest& req) const {
  return UdsNrcCodes::kOk;
}

ValidatorController::ValidatorController(/* args */) {}

ValidatorController::~ValidatorController() {}

}  // namespace uds
}  // namespace diag_demon
}  // namespace platform
}  // namespace srp
