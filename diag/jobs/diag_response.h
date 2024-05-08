#ifndef DIAG_JOBS_DIAG_RESPONSE_H_
#define DIAG_JOBS_DIAG_RESPONSE_H_

#include <algorithm>
#include <cstdint>
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
  DiagResponse(const DiagResponseCodes code) : code_{code}, payload_{} {}
  void AddSubFunction(const std::vector<uint8_t>& sub_id) {
    std::copy(sub_id.begin(), sub_id.end(), std::front_inserter(payload_));
  }

  void SetServiceId(const uint8_t s_id) { s_id_ = s_id; }
  std::vector<uint8_t> ParseToVector() const {
    if (code_ == DiagResponseCodes::kOk) {
      std::vector<uint8_t> res{s_id_};
      std::copy(payload_.begin(), payload_.end(), std::back_inserter(res));
      return std::move(res);
    } else {
      return {0x7f, s_id_, code_};
    }
  }

  virtual ~DiagResponse() = default;
};
}  // namespace diag
}  // namespace simba

#endif  // DIAG_JOBS_DIAG_RESPONSE_H_
