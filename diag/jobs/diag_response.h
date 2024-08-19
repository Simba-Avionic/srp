/**
 * @file diag_response.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef DIAG_JOBS_DIAG_RESPONSE_H_
#define DIAG_JOBS_DIAG_RESPONSE_H_

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

namespace simba {
namespace diag {

enum DiagResponseCodes : uint8_t {
  kOk = 0x00,
  kGeneralReject = 0x10,
  kSubFunctionNotSupported = 0x12,
  kInvalidMessageLengthFormat = 0x13,
  kConditionsNotCorrect = 0x22,
  kRequestSequenceError = 0x24,
  kFailurePreventsExecutionOfRequestedAction = 0x26,
  kRequestOutOfRange = 0x31,
  kSecurityAccessDenied = 0x33,
  kInvalidKey = 0x35,
  kExceededNumberOfAttempts = 0x36,
  kRequiredTimeDelayHasNotExpired = 0x37,
  kUploadDownloadNotAccepted = 0x70,
  kTransferDataSuspended = 0x71,
  kProgrammingFailure = 0x72,
  kWrongBlockSequenceCounter = 0x73,
};

class DiagResponse {
 private:
  DiagResponseCodes code_;
  std::vector<uint8_t> payload_;
  uint8_t s_id_{0x0};

 public:
  DiagResponse(const DiagResponseCodes code,
               const std::vector<uint8_t>& payload)
      : code_{code}, payload_{payload} {}
  explicit DiagResponse(const DiagResponseCodes code) : code_{code}, payload_{} {}
    DiagResponse(const std::vector<uint8_t>& payload)
      : code_{DiagResponseCodes::kOk}, payload_{payload} {}
  void AddSubFunction(const std::vector<uint8_t>& sub_id) {
    std::vector<uint8_t> temp{sub_id};
    std::copy(payload_.cbegin(), payload_.cend(), std::back_inserter(temp));
    payload_ = temp;
  }

  void SetServiceId(const uint8_t s_id) { s_id_ = s_id; }
  std::vector<uint8_t> ParseToVector() const {
    if (code_ == DiagResponseCodes::kOk) {
      std::vector<uint8_t> res{static_cast<uint8_t>(s_id_ + 0x40)};
      std::copy(payload_.begin(), payload_.end(), std::back_inserter(res));
      return res;
    } else {
      return {0x7f, s_id_, code_};
    }
  }

  virtual ~DiagResponse() = default;
};
}  // namespace diag
}  // namespace simba

#endif  // DIAG_JOBS_DIAG_RESPONSE_H_
