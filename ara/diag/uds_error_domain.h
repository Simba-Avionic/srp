/**
 * @file UDS_error_domain.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_DIAG_UDS_ERROR_DOMAIN_H_
#define ARA_DIAG_UDS_ERROR_DOMAIN_H_

#include <string>

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"

namespace ara {
namespace diag {
enum UdsDiagErrc : ara::core::ErrorDomain::CodeType {
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

class UdsErrorDomain final : public ara::core::ErrorDomain {
 public:
  using Errc = UdsDiagErrc;

  UdsErrorDomain() noexcept;
  const std::string Name() const noexcept override;
  const std::string Message(
      ara::core::ErrorDomain::CodeType errorCode) const noexcept override;
};

ara::core::ErrorCode MakeErrorCode(
    UdsDiagErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept;

ara::core::ErrorCode MakeErrorCode(UdsDiagErrc code) noexcept;

constexpr const ara::core::ErrorDomain& GetDiagDomain() noexcept;
}  // namespace diag
}  // namespace ara
#endif  // ARA_DIAG_UDS_ERROR_DOMAIN_H_
