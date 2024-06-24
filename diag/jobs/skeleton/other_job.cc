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

DiagResponse OtherJob::Method(const DiagRequest request) {
  const std::vector<uint8_t> payload = request.GetPayload(2);
  DiagResponse res{DiagResponseCodes::kGeneralReject};
  res = Callback(std::move(payload));
  res.SetServiceId(request.GetServiceID());
  res.AddSubFunction(request.GetPayload(0, 2));
  return res;
}
DiagResponse OtherJob::Callback(const std::vector<uint8_t>& payload) {
  return DiagResponse{DiagResponseCodes::kGeneralReject};
}
OtherJob::OtherJob(const std::string instance) : JobCommon(instance) {}
}  // namespace diag
}  // namespace simba
