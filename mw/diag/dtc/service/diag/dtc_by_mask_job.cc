/**
 * @file dtc_by_mask_job.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-06-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "mw/diag/dtc/service/diag/dtc_by_mask_job.h"

#include "core/logger/Logger.h"
namespace simba {
namespace mw {
namespace dtc {

DtcByMaskJob::DtcByMaskJob(const std::string& instance)
    : diag::OtherJob(instance) {}

DtcByMaskJob::~DtcByMaskJob() {}

diag::DiagResponse DtcByMaskJob::Callback(const std::vector<uint8_t>& payload) {
  if (payload.size() != 1U) {
    AppLogger::Error("Incorrect payload size!");
    return diag::DiagResponse{diag::DiagResponseCodes::kConditionsNotCorrect};
  }
  return diag::DiagResponse{diag::DiagResponseCodes::kSubFunctionNotSupported};
}

}  // namespace dtc
}  // namespace mw
}  // namespace simba