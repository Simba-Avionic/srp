#ifndef MW_DIAG_DTC_SERVICE_DIAG_DTC_LIST_JOB_H_
#define MW_DIAG_DTC_SERVICE_DIAG_DTC_LIST_JOB_H_

#include "diag/jobs/skeleton/other_job.h"

namespace simba {
namespace mw {
namespace dtc {
class DtcListJob final: public diag::OtherJob {
 private:
  diag::DiagResponse Callback(const std::vector<uint8_t>& payload) override;
 public:
  DtcListJob(const std::string& instance);
  ~DtcListJob();
};

}  // namespace dtc
}  // namespace mw
}  // namespace simba

#endif  // MW_DIAG_DTC_SERVICE_DIAG_DTC_LIST_JOB_H_
