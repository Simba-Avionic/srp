#ifndef MW_DIAG_DTC_SERVICE_DIAG_DTC_SNAPSHOT_JOB_H_
#define MW_DIAG_DTC_SERVICE_DIAG_DTC_SNAPSHOT_JOB_H_

#include "diag/jobs/skeleton/other_job.h"

namespace simba {
namespace mw {
namespace dtc {
class DtcSnapshotJob final: public diag::OtherJob {
 private:
  diag::DiagResponse Callback(const std::vector<uint8_t>& payload) override;
 public:
  DtcSnapshotJob(const std::string& instance);
  ~DtcSnapshotJob();
};

}  // namespace dtc
}  // namespace mw
}  // namespace simba

#endif // MW_DIAG_DTC_SERVICE_DIAG_DTC_SNAPSHOT_JOB_H_
