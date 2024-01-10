/**
 * @file idiag_controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-03
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef DIAG_BASE_CONTROLLER_IDIAG_CONTROLLER_H_
#define DIAG_BASE_CONTROLLER_IDIAG_CONTROLLER_H_
#include <functional>
#include <vector>

#include "core/common/error_code.h"
#include "core/results/result.h"
namespace simba {
namespace diag {

using DiagMethod = std::function<simba::core::Result<std::vector<uint8_t>>(
    const std::vector<uint8_t> payload)>;

enum DiagMethodType { WRITE = 0x00, READ = 0x01, JOB = 0x02 };

class IDiagController {
 public:
  virtual simba::core::ErrorCode AddMethod(
      const uint8_t diag_id, DiagMethod callback,
      const DiagMethodType method_type) = 0;
  virtual simba::core::ErrorCode Init() = 0;
  virtual simba::core::Result<std::vector<uint8_t>> Read(
      const uint16_t service_id, const uint8_t diag_id) = 0;
  virtual simba::core::ErrorCode Write(const uint16_t service_id,
                                       const uint8_t diag_id,
                                       const std::vector<uint8_t> payload) = 0;
  virtual simba::core::Result<std::vector<uint8_t>> Job(
      const uint16_t service_id, const uint8_t diag_id,
      const std::vector<uint8_t> payload) = 0;
  virtual ~IDiagController() {}
};
}  // namespace diag
}  // namespace simba
#endif  // DIAG_BASE_CONTROLLER_IDIAG_CONTROLLER_H_
