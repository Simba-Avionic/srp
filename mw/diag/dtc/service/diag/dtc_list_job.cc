/**
 * @file dtc_list_job.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-06-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "mw/diag/dtc/service/diag/dtc_list_job.h"

namespace simba {
namespace mw {
namespace dtc {

DtcListJob::DtcListJob(const std::string& instance): diag::OtherJob(instance) {}

DtcListJob::~DtcListJob() {}

diag::DiagResponse DtcListJob::Callback(const std::vector<uint8_t>& payload) {
    return diag::DiagResponse{diag::DiagResponseCodes::kSubFunctionNotSupported};
}

}  // namespace dtc
}  // namespace mw
}  // namespace simba