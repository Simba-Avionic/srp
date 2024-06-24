/**
 * @file i_validator.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_DIAG_OTA_CODE_APPLICATION_SERVICES_I_VALIDATOR_H_
#define APPS_DIAG_OTA_CODE_APPLICATION_SERVICES_I_VALIDATOR_H_

#include "apps/diag_ota/code/application/services/uds_nrc_codes.h"
#include "apps/diag_ota/code/application/services/uds_request.h"
namespace simba {
namespace diag_ota {
namespace uds {

class IValidator {
 public:
  virtual UdsNrcCodes Validate(const UdsRequest& req) const = 0;
  virtual ~IValidator() = default;
};
}  // namespace uds
}  // namespace diag_ota
}  // namespace simba

#endif // APPS_DIAG_OTA_CODE_APPLICATION_SERVICES_I_VALIDATOR_H_
