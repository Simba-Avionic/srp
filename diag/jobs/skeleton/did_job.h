/**
 * @file did_job.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef DIAG_JOBS_SKELETON_DID_JOB_H_
#define DIAG_JOBS_SKELETON_DID_JOB_H_

#include <string>
#include <vector>

#include "diag/jobs/job_common.h"
namespace srp {
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
  explicit DiDJob(const ara::core::InstanceSpecifier& instance);
  virtual ~DiDJob() = default;
};
}  // namespace diag
}  // namespace srp

#endif  // DIAG_JOBS_SKELETON_DID_JOB_H_
