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
#include "diag/jobs/skeleton/did_job.h"

#include <utility>

namespace srp {
namespace diag {

DiagResponse DiDJob::Method(const DiagRequest request) {
  const std::vector<uint8_t> payload = request.GetPayload(2);
  DiagResponse res{DiagResponseCodes::kGeneralReject};
  if (request.GetServiceID() == 0x22) {
    if (payload.size() == 0) {
      res = Read();
    } else {
      res = DiagResponse{DiagResponseCodes::kConditionsNotCorrect};
    }
  } else if (request.GetServiceID() == 0x2E) {
    if (payload.size() == 0) {
      res = DiagResponse{DiagResponseCodes::kConditionsNotCorrect};
    } else {
      res = Write(std::move(payload));
    }
  } else {
    res = DiagResponse{DiagResponseCodes::kGeneralReject};
  }
  res.SetServiceId(request.GetServiceID());
  res.AddSubFunction(request.GetPayload(0, 2));
  return res;
}
DiagResponse DiDJob::Read() {
  return DiagResponse{DiagResponseCodes::kRequestOutOfRange};
}
DiagResponse DiDJob::Write(const std::vector<uint8_t>& payload) {
  return DiagResponse{DiagResponseCodes::kRequestOutOfRange};
}
DiDJob::DiDJob(const ara::core::InstanceSpecifier& instance) : JobCommon(instance) {}
}  // namespace diag
}  // namespace srp
