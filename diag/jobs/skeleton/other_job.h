/**
 * @file other_job.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef DIAG_JOBS_SKELETON_OTHER_JOB_H_
#define DIAG_JOBS_SKELETON_OTHER_JOB_H_

#include <string>
#include <vector>

#include "diag/jobs/job_common.h"
namespace simba {
namespace diag {
class OtherJob : public JobCommon {
 private:
  DiagResponse Method(const DiagRequest request) override;
  const uint8_t subservice_length_;
 protected:
  /**
   * @brief Callback for other UDS job
   *
   * @return DiagResponse
   */
  virtual DiagResponse Callback(const std::vector<uint8_t>& payload);
  virtual std::vector<uint8_t> GetSubServiceId(const DiagRequest& request) const;
 public:
  explicit OtherJob(const std::string instance, uint8_t subservice_length);
  virtual ~OtherJob() = default;
};
}  // namespace diag
}  // namespace simba

#endif  // DIAG_JOBS_SKELETON_OTHER_JOB_H_
