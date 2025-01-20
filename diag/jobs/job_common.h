/**
 * @file job_common.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-05-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef DIAG_JOBS_JOB_COMMON_H_
#define DIAG_JOBS_JOB_COMMON_H_
#include <cstdint>
#include <memory>
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "ara/core/instance_specifier.h"
#include "ara/log/logger.h"
#include "communication-core/sockets/Isocket_stream.h"
#include "diag/jobs/diag_request.h"
#include "diag/jobs/diag_response.h"
namespace srp {
namespace diag {
class JobCommon {
 private:
  std::vector<uint8_t> rx_callback(const std::string&, const std::uint16_t&,
                                   std::vector<std::uint8_t> payload);
  std::unique_ptr<srp::com::soc::ISocketStream> soc_;
  std::string job_id_{""};

 protected:
  const ara::core::InstanceSpecifier instance_;
  virtual DiagResponse Method(const DiagRequest request) = 0;
  const ara::log::Logger& diag_logger;

 public:
  void StartOffer();
  void StopOffer();
  explicit JobCommon(const ara::core::InstanceSpecifier& instance);
  virtual ~JobCommon() = default;
};
}  // namespace diag
}  // namespace srp

#endif  // DIAG_JOBS_JOB_COMMON_H_
