/**
 * @file dtcService.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "mw/diag/dtc/service/dtcService.h"

#include <unordered_map>
#include <vector>

namespace simba {
namespace mw {
namespace dtc {

DtcService::DtcService() {}
core::ErrorCode DtcService::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  dtc_list_ = std::make_unique<DtcListJob>("DtcService/DtcList");
  dtc_by_mask_job_ = std::make_unique<DtcByMaskJob>("DtcService/DtcByMask");
  dtc_snapshot_job_ =
      std::make_unique<DtcSnapshotJob>("DtcService/dtc_snapshot");
  return core::ErrorCode::kOk;
}
core::ErrorCode DtcService::Run(std::stop_token token) {
  dtc_list_->StartService();
  dtc_by_mask_job_->StartService();
  dtc_snapshot_job_->StartService();
  AppLogger::Info("App started");
  this->SleepMainThread();
  return core::ErrorCode::kOk;
}

}  // namespace dtc
}  // namespace mw
}  // namespace simba
