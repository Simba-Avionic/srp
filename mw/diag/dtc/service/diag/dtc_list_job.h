#ifndef MW_DIAG_DTC_SERVICE_DIAG_DTC_LIST_JOB_H_
#define MW_DIAG_DTC_SERVICE_DIAG_DTC_LIST_JOB_H_

#include <memory>

#include "diag/jobs/skeleton/other_job.h"
#include "mw/diag/dtc/service/database.h"

namespace simba {
namespace mw {
namespace dtc {
class DtcListJob final : public diag::OtherJob {
 private:
  const std::shared_ptr<DataBase> db_;
  diag::DiagResponse Callback(const std::vector<uint8_t>& payload) override;

 public:
  DtcListJob(const std::string& instance, std::shared_ptr<DataBase> db);
  ~DtcListJob();
};

}  // namespace dtc
}  // namespace mw
}  // namespace simba

#endif  // MW_DIAG_DTC_SERVICE_DIAG_DTC_LIST_JOB_H_
