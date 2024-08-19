/**
 * @file dtc_snapshot_job.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-06-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "mw/diag/dtc/service/diag/dtc_snapshot_job.h"

#include "core/logger/Logger.h"

namespace simba {
namespace mw {
namespace dtc {

DtcSnapshotJob::DtcSnapshotJob(const std::string& instance)
    : diag::OtherJob(instance,1U) {}

DtcSnapshotJob::~DtcSnapshotJob() {}

diag::DiagResponse DtcSnapshotJob::Callback(
    const std::vector<uint8_t>& payload) {
  if (payload.size() != 3U) {
    AppLogger::Error("Incorrect payload size!");
    return diag::DiagResponse{diag::DiagResponseCodes::kConditionsNotCorrect};
  }
  return diag::DiagResponse{diag::DiagResponseCodes::kSubFunctionNotSupported};
}

}  // namespace dtc
}  // namespace mw
}  // namespace simba
