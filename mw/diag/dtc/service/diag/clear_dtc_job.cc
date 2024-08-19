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

#include "mw/diag/dtc/service/diag/clear_dtc_job.h"

#include "core/logger/Logger.h"

namespace simba {
namespace mw {
namespace dtc {

ClearDtcJob::ClearDtcJob(const std::string& instance,
                           std::shared_ptr<DataBase> db)
    : diag::OtherJob(instance,0U), db_{db} {}

ClearDtcJob::~ClearDtcJob() {}


diag::DiagResponse ClearDtcJob::Callback(const std::vector<uint8_t>& payload) {
  if (payload.size() != 4U) {
    std::string temp{""};
    for(const auto& i:payload){
      temp+=std::to_string(i)+"|";
    }
    AppLogger::Error("Incorrect payload size! Size: "+std::to_string(payload.size())+" Value: "+temp);
    return diag::DiagResponse{diag::DiagResponseCodes::kConditionsNotCorrect};
  }

  return diag::DiagResponse{{}};
}

}  // namespace dtc
}  // namespace mw
}  // namespace simba