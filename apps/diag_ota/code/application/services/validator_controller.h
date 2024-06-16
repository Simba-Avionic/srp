/**
 * @file validator_controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-04-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef APPS_DIAG_OTA_CODE_APPLICATION_SERVICES_VALIDATOR_CONTROLLER_H_
#define APPS_DIAG_OTA_CODE_APPLICATION_SERVICES_VALIDATOR_CONTROLLER_H_
#include "apps/diag_ota/code/application/services/i_validator.h"
namespace simba {
namespace diag_ota {
namespace uds {

class ValidatorController : public IValidator {
 private:
  /* data */
 public:
  UdsNrcCodes Validate(const UdsRequest& req) const override;
  ValidatorController(/* args */);
  ~ValidatorController();
};
}  // namespace uds
}  // namespace diag_ota
}  // namespace simba

#endif  // APPS_DIAG_OTA_CODE_APPLICATION_SERVICES_VALIDATOR_CONTROLLER_H_
