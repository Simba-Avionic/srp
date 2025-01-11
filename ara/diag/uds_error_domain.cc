/**
 * @file diag_error_domain.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/diag/uds_error_domain.h"

#include <string>
#include <unordered_map>

#include "ara/core/string_view.h"

namespace ara {
namespace diag {
namespace {
std::unordered_map<ara::core::ErrorDomain::CodeType, ara::core::StringView>
    kKeyList{
        {UdsDiagErrc::kGeneralReject, "kGeneralReject"},
        {UdsDiagErrc::kSubFunctionNotSupported, "kSubFunctionNotSupported"},
        {UdsDiagErrc::kInvalidMessageLengthFormat,
         "kInvalidMessageLengthFormat"},
        {UdsDiagErrc::kConditionsNotCorrect, "kConditionsNotCorrect"},
        {UdsDiagErrc::kRequestSequenceError, "kRequestSequenceError"},
        {UdsDiagErrc::kFailurePreventsExecutionOfRequestedAction,
         "kFailurePreventsExecutionOfRequestedAction"},
        {UdsDiagErrc::kRequestOutOfRange, "kRequestOutOfRange"},
        {UdsDiagErrc::kSecurityAccessDenied, "kSecurityAccessDenied"},
        {UdsDiagErrc::kInvalidKey, "kInvalidKey"},
        {UdsDiagErrc::kExceededNumberOfAttempts, "kExceededNumberOfAttempts"},
        {UdsDiagErrc::kRequiredTimeDelayHasNotExpired,
         "kRequiredTimeDelayHasNotExpired"},
        {UdsDiagErrc::kUploadDownloadNotAccepted, "kUploadDownloadNotAccepted"},
        {UdsDiagErrc::kTransferDataSuspended, "kTransferDataSuspended"},
        {UdsDiagErrc::kProgrammingFailure, "kProgrammingFailure"},
        {UdsDiagErrc::kWrongBlockSequenceCounter, "kWrongBlockSequenceCounter"},
    };
const UdsErrorDomain domain_;
}  // namespace

UdsErrorDomain::UdsErrorDomain() noexcept
    : ara::core::ErrorDomain{0x8000000000000404} {}
const std::string UdsErrorDomain::Name() const noexcept {
  return std::string{"CoreErrorDomain"};
}
const std::string UdsErrorDomain::Message(
    ara::core::ErrorDomain::CodeType errorCode) const noexcept {
  return kKeyList[errorCode];
}

ara::core::ErrorCode MakeErrorCode(
    UdsDiagErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode{
      static_cast<ara::core::ErrorDomain::CodeType>(code), GetDiagDomain(),
      data};
}

ara::core::ErrorCode MakeErrorCode(UdsDiagErrc code) noexcept {
  return MakeErrorCode(code, "");
}

constexpr const ara::core::ErrorDomain& GetDiagDomain() noexcept {
  return domain_;
}
}  // namespace diag
}  // namespace ara
