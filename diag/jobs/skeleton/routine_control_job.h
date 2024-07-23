/**
 * @file routine_control_job.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-05-09
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef DIAG_JOBS_SKELETON_ROUTINE_CONTROL_JOB_H_
#define DIAG_JOBS_SKELETON_ROUTINE_CONTROL_JOB_H_

#include <string>
#include <vector>

#include "diag/jobs/job_common.h"
namespace simba {
namespace diag {
class RoutineControlJob : public JobCommon {
 private:
  DiagResponse Method(const DiagRequest request) override;

 protected:
/**
 * @brief Callback for start (0x31_01_<job_id>)
 * 
 * @param payload 
 * @return DiagResponse 
 */
virtual DiagResponse Start(const std::vector<uint8_t>& payload);
/**
 * @brief Callback for Stop (0x31_02_<job_id>)
 * 
 * @param payload 
 * @return DiagResponse 
 */
virtual DiagResponse Stop(const std::vector<uint8_t>& payload);
/**
 * @brief Callback for GetStatus (0x31_03_<job id>)
 * 
 * @param payload 
 * @return DiagResponse 
 */
virtual DiagResponse GetResponse(const std::vector<uint8_t>& payload);

 public:
  explicit RoutineControlJob(const std::string instance);
  virtual ~RoutineControlJob() = default;
};
}  // namespace diag
}  // namespace simba

#endif  // DIAG_JOBS_SKELETON_ROUTINE_CONTROL_JOB_H_
