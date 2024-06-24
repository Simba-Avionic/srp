/**
 * @file read_vin_did.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef APPS_DIAG_OTA_CODE_APPLICATION_PROXY_READ_VIN_DID_H_
#define APPS_DIAG_OTA_CODE_APPLICATION_PROXY_READ_VIN_DID_H_
#include <string>

#include "core/logger/Logger.h"
#include "diag/jobs/skeleton/did_job.h"
namespace simba {
namespace diag_ota {
class ReadVinDiD : public diag::DiDJob {
 private:
  /**
   * @brief Callback for 0x22 UDS job (Read data by id) *optional
   *
   * @return DiagResponse
   */
  diag::DiagResponse Read() {
    AppLogger::Info("Działa");
    return diag::DiagResponse(diag::DiagResponseCodes::kOk,
                              {0x10, 0x11, 0x22, 0x33});
  }

 public:
  explicit ReadVinDiD(const std::string instance) : diag::DiDJob(instance) {}
};
}  // namespace diag_ota
}  // namespace simba
#endif // APPS_DIAG_OTA_CODE_APPLICATION_PROXY_READ_VIN_DID_H_
