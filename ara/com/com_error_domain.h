#ifndef ARA_COM_DIAG_ERROR_DOMAIN_H_
#define ARA_COM_DIAG_ERROR_DOMAIN_H_
#include "ara/core/error_domain.h"
#include "ara/core/error_code.h"

namespace ara {
namespace com {
enum class ComErrc : ara::core::ErrorDomain::CodeType {
  kAlreadyOffered = 101,
  kConfigurationMismatch = 102,
  kInvalidArgument = 105,
  kNotOffered = 106,
  kNotOffered = 107,
  kBusy = 109,
  kFailed = 110,
  kMemoryError = 111,
};

enum class ComOfferErrc : ara::core::ErrorDomain::CodeType {
  kAlreadyOffered = 101,
  kConfigurationMismatch = 102
};

class ComErrorDomain final : public ara::core::ErrorDomain {
 public:
  using Errc = ComErrc;

  constexpr ComErrorDomain() noexcept;
  const char* Name() const noexcept override;
  const char* Message(
      ara::core::ErrorDomain::CodeType errorCode) const noexcept override;
};

constexpr ara::core::ErrorCode MakeErrorCode(
    ComErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept;

constexpr ara::core::ErrorCode MakeErrorCode(
    ComOfferErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept;

constexpr const ara::core::ErrorDomain& GetDiagDomain() noexcept;
}  // namespace diag
}  // namespace ara
#endif // ARA_COM_DIAG_ERROR_DOMAIN_H_
