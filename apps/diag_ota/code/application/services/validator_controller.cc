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
#include "apps/diag_ota/code/application/services/validator_controller.h"

namespace simba {
namespace diag_ota {
namespace uds {

UdsNrcCodes ValidatorController::Validate(const UdsRequest& req) const {
  return UdsNrcCodes::kOk;
}

ValidatorController::ValidatorController(/* args */) {}

ValidatorController::~ValidatorController() {}

}  // namespace uds
}  // namespace diag_ota
}  // namespace simba
