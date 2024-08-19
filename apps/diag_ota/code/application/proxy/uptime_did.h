/**
 * @file uptime_did.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-09
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef APPS_DIAG_OTA_CODE_APPLICATION_PROXY_UPTIME_DID_H_
#define APPS_DIAG_OTA_CODE_APPLICATION_PROXY_UPTIME_DID_H_

#include <time.h>

#include <string>

#include "core/logger/Logger.h"
#include "diag/jobs/skeleton/did_job.h"
#include "core/common/endianess_converter.h"

namespace simba {
namespace diag_ota {
class UptimeDiD : public diag::DiDJob {
 private:
  /**
   * @brief Callback for 0x22 UDS job (Read data by id) *optional
   *
   * @return DiagResponse
   */
  diag::DiagResponse Read() {
    timespec t;
    clock_gettime(CLOCK_BOOTTIME, &t);
    std::uint32_t data =  core::converter::EndianessConverter::Convert(static_cast<uint32_t>(t.tv_sec));
    const std::vector<std::uint8_t> result(
        (std::uint8_t*)&data, (std::uint8_t*)&(data) + sizeof(std::uint32_t));
    return diag::DiagResponse(diag::DiagResponseCodes::kOk,
                             result);
  }

 public:
  explicit UptimeDiD(const std::string instance) : diag::DiDJob(instance) {}
};
}  // namespace diag_ota
}  // namespace simba

#endif // APPS_DIAG_OTA_CODE_APPLICATION_PROXY_UPTIME_DID_H_
