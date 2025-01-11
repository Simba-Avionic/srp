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
#include "ara/com/com_error_domain.h"

#include <string>
#include <unordered_map>

#include "ara/core/string_view.h"

namespace ara {
namespace com {
namespace {
std::unordered_map<ara::core::ErrorDomain::CodeType, std::string> kKeyList{
    {static_cast<ara::core::ErrorDomain::CodeType>(
         ComErrc::kServiceNotAvailable),
     "Service Not Available"},
    {static_cast<ara::core::ErrorDomain::CodeType>(ComErrc::kMaxSamplesReached),
     "Max Samples Reached"},
    {static_cast<ara::core::ErrorDomain::CodeType>(
         ComErrc::kNetworkBindingFailure),
     "Network Binding Failure"},
    {static_cast<ara::core::ErrorDomain::CodeType>(
         ComErrc::kGrantEnforcementError),
     "Grant Enforcement Error"},
    {static_cast<ara::core::ErrorDomain::CodeType>(ComErrc::kPeerIsUnreachable),
     "Peer Is Unreachable"},
    {static_cast<ara::core::ErrorDomain::CodeType>(
         ComErrc::kFieldValueIsNotValid),
     "Field Value Is Not Valid"},
    {static_cast<ara::core::ErrorDomain::CodeType>(ComErrc::kSetHandlerNotSet),
     "Set Handler Not Set"},
    {static_cast<ara::core::ErrorDomain::CodeType>(ComErrc::kUnsetFailure),
     "Unset Failure"},
    {static_cast<ara::core::ErrorDomain::CodeType>(
         ComErrc::kSampleAllocationFailure),
     "Sample Allocation Failure"},
    {static_cast<ara::core::ErrorDomain::CodeType>(
         ComErrc::kIllegalUseOfAllocate),
     "Illegal Use Of Allocate"},
    {static_cast<ara::core::ErrorDomain::CodeType>(ComErrc::kServiceNotOffered),
     "Service Not Offered"},
    {static_cast<ara::core::ErrorDomain::CodeType>(
         ComErrc::kCommunicationLinkError),
     "Communication Link Error"},
    {static_cast<ara::core::ErrorDomain::CodeType>(ComErrc::kNoClients),
     "No Clients"},
    {static_cast<ara::core::ErrorDomain::CodeType>(
         ComErrc::kCommunicationStackError),
     "Communication Stack Error"},
    {static_cast<ara::core::ErrorDomain::CodeType>(
         ComErrc::kInstanceIDCouldNotBeResolved),
     "Instance ID Could Not Be Resolved"},
    {static_cast<ara::core::ErrorDomain::CodeType>(
         ComErrc::kMaxSampleCountNotRealizable),
     "Max Sample Count Not Realizable"},
    {static_cast<ara::core::ErrorDomain::CodeType>(
         ComErrc::kWrongMethodCallProcessingMode),
     "Wrong Method Call Processing Mode"},
    {static_cast<ara::core::ErrorDomain::CodeType>(
         ComErrc::kErroneousFileHandle),
     "Erroneous File Handle"}};
const ComErrorDomain domain_;
}  // namespace

ComErrorDomain::ComErrorDomain() noexcept
    : ara::core::ErrorDomain{0x8000000000001267} {}
const std::string ComErrorDomain::Name() const noexcept {
  return std::string{"ComErrorDomain"};
}
const std::string ComErrorDomain::Message(
    ara::core::ErrorDomain::CodeType errorCode) const noexcept {
  return kKeyList[errorCode];
}

ara::core::ErrorCode MakeErrorCode(
    ComErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode{
      static_cast<ara::core::ErrorDomain::CodeType>(code), GetComDomain(),
      data};
}

ara::core::ErrorCode MakeErrorCode(
    ComOfferErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode{
      static_cast<ara::core::ErrorDomain::CodeType>(code), GetComDomain(),
      data};
}
ara::core::ErrorCode MakeErrorCode(
    ara::com::someip::MessageCode code,
    ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode{
      static_cast<ara::core::ErrorDomain::CodeType>(code), GetComDomain(),
      data};
}
const ara::core::ErrorDomain& GetComDomain() noexcept { return domain_; }
}  // namespace com
}  // namespace ara
