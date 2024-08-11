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
#ifndef PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_EXAMPLE_DID_H_
#define PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_EXAMPLE_DID_H_
#include <string>

#include "ara/log/log.h"
#include "diag/jobs/skeleton/did_job.h"
namespace simba {
namespace platform {
namespace diag_demon {

class ExampleDiD : public diag::DiDJob {
 private:
  /**
   * @brief Callback for 0x22 UDS job (Read data by id) *optional
   *
   * @return DiagResponse
   */
  diag::DiagResponse Read() {
    ara::log::LogInfo() << "Działa";
    return diag::DiagResponse(diag::DiagResponseCodes::kOk,
                              {0x10, 0x11, 0x22, 0x33});
  }

 public:
  explicit ExampleDiD(const ara::core::InstanceSpecifier& instance)
      : diag::DiDJob(instance) {}
};
}  // namespace diag_demon
}  // namespace platform
}  // namespace simba

#endif  // PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_EXAMPLE_DID_H_
