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

#include "mw/diag/dtc/service/lib/converter.h"

namespace simba {
namespace mw {
namespace dtc {




DtcListJob::DtcListJob(const std::string& instance,
                       std::shared_ptr<DataBase> db)
    : diag::OtherJob(instance,1U), db_{db} {}

DtcListJob::~DtcListJob() {}

diag::DiagResponse DtcListJob::Callback(const std::vector<uint8_t>& payload) {
  std::vector<uint8_t> res{static_cast<uint8_t>(db_->GetAllowedMask())};
  for(const auto& dtc: db_->GetDataBase()){
    const auto temp_dtc = converter::VectorConverter::Convert(dtc.second.id_,true);
    res.insert(res.end(),temp_dtc.begin(),temp_dtc.end());
    res.push_back(dtc.second.status_);
  }
  return diag::DiagResponse{res};
}

}  // namespace dtc
}  // namespace mw
}  // namespace simba