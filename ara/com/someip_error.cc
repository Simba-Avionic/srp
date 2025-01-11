/**
 * @file com_error_domain.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/com/someip_error.h"

#include <unordered_map>

#include "ara/core/string_view.h"

namespace ara {
namespace com {
namespace someip {
namespace {
std::unordered_map<ara::core::ErrorDomain::CodeType, ara::core::StringView>
    kKeyList{{static_cast<ara::core::ErrorDomain::CodeType>(
                  someip::MessageCode::kENotOk),
              "kENotOk"},
             {static_cast<ara::core::ErrorDomain::CodeType>(
                  someip::MessageCode::kEUnknownService),
              "kEUnknownService"},
             {static_cast<ara::core::ErrorDomain::CodeType>(
                  someip::MessageCode::kEUnknownMethod),
              "kEUnknownMethod"},
             {static_cast<ara::core::ErrorDomain::CodeType>(
                  someip::MessageCode::kENotReady),
              "kENotReady"},
             {static_cast<ara::core::ErrorDomain::CodeType>(
                  someip::MessageCode::kENotReachable),
              "kENotReachable"},
             {static_cast<ara::core::ErrorDomain::CodeType>(
                  someip::MessageCode::kETimeout),
              "kETimeout"},
             {static_cast<ara::core::ErrorDomain::CodeType>(
                  someip::MessageCode::kEWrongProtocolVersion),
              "kEWrongProtocolVersion"},
             {static_cast<ara::core::ErrorDomain::CodeType>(
                  someip::MessageCode::kEWrongInterfaceVersion),
              "kEWrongInterfaceVersion"},
             {static_cast<ara::core::ErrorDomain::CodeType>(
                  someip::MessageCode::kEMalformedMessage),
              "kEMalformedMessage"}};
const SomeIpErrorDomain domain_;
}  // namespace

SomeIpErrorDomain::SomeIpErrorDomain() noexcept
    : ara::core::ErrorDomain{0x8900000000001267} {}
const std::string SomeIpErrorDomain::Name() const noexcept {
  return ara::core::StringView{"SomeIpErrorDomain"};
}
const std::string SomeIpErrorDomain::Message(
    ara::core::ErrorDomain::CodeType errorCode) const noexcept {
  return kKeyList[errorCode];
}

ara::core::ErrorCode MakeErrorCode(
    SomeIpErrorDomain::Errc code,
    ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode{
      static_cast<ara::core::ErrorDomain::CodeType>(code), GetComDomain(),
      data};
}

const ara::core::ErrorDomain& GetComDomain() noexcept { return domain_; }
}  // namespace someip
}  // namespace com
}  // namespace ara
