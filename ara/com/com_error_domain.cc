#include "ara/com/com_error_domain.h"

#include <unordered_map>

#include "ara/core/string_view.h"

namespace ara {
namespace com {
namespace {
std::unordered_map<ara::core::ErrorDomain::CodeType, ara::core::StringView>
    kKeyList{
        {static_cast<ara::core::ErrorDomain::CodeType>(
             ComErrc::kAlreadyOffered),
         "Already Offered"},
        {static_cast<ara::core::ErrorDomain::CodeType>(ComErrc::kBusy), "Busy"},
        {static_cast<ara::core::ErrorDomain::CodeType>(
             ComErrc::kConfigurationMismatch),
         "Configuration Mismatch"},
        {static_cast<ara::core::ErrorDomain::CodeType>(ComErrc::kFailed),
         "Failed"},
        {static_cast<ara::core::ErrorDomain::CodeType>(
             ComErrc::kInvalidArgument),
         "Invalid Argument"},
        {static_cast<ara::core::ErrorDomain::CodeType>(ComErrc::kMemoryError),
         "Memory Error"},
        {static_cast<ara::core::ErrorDomain::CodeType>(ComErrc::kNotOffered),
         "Not Offered"}};
constexpr const ComErrorDomain domain_;
}  // namespace

constexpr ComErrorDomain::ComErrorDomain() noexcept
    : ara::core::ErrorDomain{0x8000000000000403} {}
const char* ComErrorDomain::Name() const noexcept {
  return ara::core::StringView{"CoreErrorDomain"}.c_str();
}
const char* ComErrorDomain::Message(
    ara::core::ErrorDomain::CodeType errorCode) const noexcept {
  return kKeyList[errorCode].c_str();
}

constexpr ara::core::ErrorCode MakeErrorCode(
    ComErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode{
      static_cast<ara::core::ErrorDomain::CodeType>(code), GetDiagDomain(),
      data};
}

constexpr ara::core::ErrorCode MakeErrorCode(
    ComOfferErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode{
      static_cast<ara::core::ErrorDomain::CodeType>(code), GetDiagDomain(),
      data};
}

constexpr const ara::core::ErrorDomain& GetDiagDomain() noexcept {
  return domain_;
}
}  // namespace com
}  // namespace ara
