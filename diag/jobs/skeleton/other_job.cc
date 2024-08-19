/**
 * @file did_job.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief Read and write data by id
 * @version 0.1
 * @date 2024-05-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "diag/jobs/skeleton/other_job.h"
#include <utility>

namespace simba {
namespace diag {

std::vector<uint8_t> OtherJob::GetSubServiceId(const DiagRequest& request) const {
  return request.GetPayload(0, subservice_length_);
}

DiagResponse OtherJob::Method(const DiagRequest request) {
  const std::vector<uint8_t> payload = request.GetPayload(subservice_length_);
  DiagResponse res = Callback(payload);
  res.SetServiceId(request.GetServiceID());
  res.AddSubFunction(this->GetSubServiceId(request));
  return res;
}
DiagResponse OtherJob::Callback(const std::vector<uint8_t>& payload) {
  return DiagResponse{DiagResponseCodes::kGeneralReject};
}
OtherJob::OtherJob(const std::string instance, uint8_t subservice_length) : JobCommon(instance),subservice_length_{subservice_length} {}
}  // namespace diag
}  // namespace simba
