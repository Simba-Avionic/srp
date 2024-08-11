#ifndef ARA_CORE_CORE_ERROR_DOMAIN_H_
#define ARA_CORE_CORE_ERROR_DOMAIN_H_

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"

namespace ara {
namespace core {
enum class CoreErrc : ErrorDomain::CodeType {
  kInvalidArgument = 22,
  kInvalidMetaModelShortname = 137,
  kInvalidMetaModelPath = 138
};
class CoreErrorDomain final : public ErrorDomain {
 public:
  using Errc = CoreErrc;
  CoreErrorDomain() noexcept;
  const char* Name() const noexcept override;
  const char* Message(ErrorDomain::CodeType errorCode) const noexcept override;
};
ErrorCode MakeErrorCode(CoreErrc code,
                        ErrorDomain::SupportDataType data) noexcept;
constexpr const ara::core::ErrorDomain& GetDiagDomain() noexcept;
}  // namespace core
}  // namespace ara
#endif  // ARA_CORE_CORE_ERROR_DOMAIN_H_
