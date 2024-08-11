#ifndef ARA_DIAG_DIAG_ERROR_DOMAIN_H_
#define ARA_DIAG_DIAG_ERROR_DOMAIN_H_
#include "ara/core/error_domain.h"
#include "ara/core/error_code.h"

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

  constexpr DiagErrorDomain() noexcept;
  const char* Name() const noexcept override;
  const char* Message(
      ara::core::ErrorDomain::CodeType errorCode) const noexcept override;
};

constexpr ara::core::ErrorCode MakeErrorCode(
    DiagErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept;

constexpr ara::core::ErrorCode MakeErrorCode(
    DiagOfferErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept;

constexpr const ara::core::ErrorDomain& GetDiagDomain() noexcept;
}  // namespace diag
}  // namespace ara
#endif // ARA_DIAG_DIAG_ERROR_DOMAIN_H_
