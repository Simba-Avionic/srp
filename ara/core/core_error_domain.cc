/**
 * @file core_error_domain.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/core/core_error_domain.h"

#include <unordered_map>

#include "ara/core/string_view.h"
namespace ara {
namespace core {
namespace {
std::unordered_map<ErrorDomain::CodeType, StringView> kKeyList{
    {static_cast<ErrorDomain::CodeType>(CoreErrc::kInvalidArgument),
     "Invalid Argument"},
    {static_cast<ErrorDomain::CodeType>(CoreErrc::kInvalidMetaModelShortname),
     "Invalid Meta Model Shortname"},
    {static_cast<ErrorDomain::CodeType>(CoreErrc::kInvalidMetaModelPath),
     "Invalid Meta Model Path"}};
const CoreErrorDomain domain_;
}  // namespace

CoreErrorDomain::CoreErrorDomain() noexcept : ErrorDomain{0x01} {}
const char* CoreErrorDomain::Name() const noexcept {
  return ara::core::StringView{"CoreErrorDomain"}.c_str();
}
const char* CoreErrorDomain::Message(
    ErrorDomain::CodeType errorCode) const noexcept {
  return kKeyList[errorCode].c_str();
}
ErrorCode MakeErrorCode(CoreErrc code,
                        ErrorDomain::SupportDataType data) noexcept {
  return ErrorCode{static_cast<ErrorDomain::CodeType>(code), GetCoreDomain(),
                   data};
}

constexpr const ara::core::ErrorDomain& GetCoreDomain() noexcept {
  return domain_;
}

}  // namespace core
}  // namespace ara
