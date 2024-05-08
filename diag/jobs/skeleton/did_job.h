#ifndef DIAG_JOBS_SKELETON_DID_JOB_H_
#define DIAG_JOBS_SKELETON_DID_JOB_H_

#include <string>
#include <vector>

#include "diag/jobs/job_common.h"
namespace simba {
namespace diag {
class DiDJob : public JobCommon {
 private:
  DiagResponse Method(const DiagRequest request) override;

 protected:
  /**
   * @brief Callback for 0x22 UDS job (Read data by id) *optional
   *
   * @return DiagResponse
   */
  virtual DiagResponse Read();
  /**
   * @brief Callback for 0x2E UDS job (Write data by id) *optional
   *
   * @param payload
   * @return DiagResponse
   */
  virtual DiagResponse Write(const std::vector<uint8_t>& payload);

 public:
  DiDJob(const std::string instance);
  virtual ~DiDJob() = default;
};
}  // namespace diag
}  // namespace simba

#endif  // DIAG_JOBS_SKELETON_DID_JOB_H_
