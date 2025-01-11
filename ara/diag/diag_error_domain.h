/**
 * @file diag_error_domain.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_DIAG_DIAG_ERROR_DOMAIN_H_
#define ARA_DIAG_DIAG_ERROR_DOMAIN_H_

#include <string>

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"

namespace ara {
namespace diag {
enum class DiagErrc : ara::core::ErrorDomain::CodeType {
  kAlreadyOffered = 101,
  kConfigurationMismatch = 102,
  kDebouncingConfigurationInconsistent = 103,
  kReportIgnored = 104,
  kInvalidArgument = 105,
  kNotOffered = 106,
  kNoSuchDTC = 108,
  kBusy = 109,
  kFailed = 110,
  kMemoryError = 111,
  kWrongDtc = 112,
  kRejected = 113,
  kResetTypeNotSupported = 114,
  kRequestFailed = 115,
  kCustomResetTypeNotSupported = 116,
  kSuppressionIgnored = 117
};

enum class DiagOfferErrc : ara::core::ErrorDomain::CodeType {
  kAlreadyOffered = 101,
  kConfigurationMismatch = 102,
  kDebouncingConfigurationInconsistent = 103
};

class DiagErrorDomain final : public ara::core::ErrorDomain {
 public:
  using Errc = DiagErrc;

  DiagErrorDomain() noexcept;
  const std::string Name() const noexcept override;
  const std::string Message(
      ara::core::ErrorDomain::CodeType errorCode) const noexcept override;
};

ara::core::ErrorCode MakeErrorCode(
    DiagErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept;

ara::core::ErrorCode MakeErrorCode(
    DiagOfferErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept;

constexpr const ara::core::ErrorDomain& GetDiagDomain() noexcept;
}  // namespace diag
}  // namespace ara
#endif  // ARA_DIAG_DIAG_ERROR_DOMAIN_H_
