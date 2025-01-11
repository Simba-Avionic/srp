/**
 * @file com_error_domain.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_COM_COM_ERROR_DOMAIN_H_
#define ARA_COM_COM_ERROR_DOMAIN_H_
#include <string>

#include "ara/com/someip/message_code.h"
#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"

namespace ara {
namespace com {
enum class ComErrc : ara::core::ErrorDomain::CodeType {
  kServiceNotAvailable = 1,
  kMaxSamplesReached = 2,
  kNetworkBindingFailure = 3,
  kGrantEnforcementError = 4,
  kPeerIsUnreachable = 5,
  kFieldValueIsNotValid = 6,
  kSetHandlerNotSet = 7,
  kUnsetFailure = 8,
  kSampleAllocationFailure = 9,
  kIllegalUseOfAllocate = 10,
  kServiceNotOffered = 11,
  kCommunicationLinkError = 12,
  kNoClients = 13,
  kCommunicationStackError = 14,
  kInstanceIDCouldNotBeResolved = 15,
  kMaxSampleCountNotRealizable = 16,
  kWrongMethodCallProcessingMode = 17,
  kErroneousFileHandle = 18
};

enum class ComOfferErrc : ara::core::ErrorDomain::CodeType {
  kAlreadyOffered = 101,
  kConfigurationMismatch = 102
};

class ComErrorDomain final : public ara::core::ErrorDomain {
 public:
  using Errc = ComErrc;

  ComErrorDomain() noexcept;
  const std::string Name() const noexcept override;
  const std::string Message(
      ara::core::ErrorDomain::CodeType errorCode) const noexcept override;
};

ara::core::ErrorCode MakeErrorCode(
    ComErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept;

ara::core::ErrorCode MakeErrorCode(
    ComOfferErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept;
ara::core::ErrorCode MakeErrorCode(
    ara::com::someip::MessageCode code,
    ara::core::ErrorDomain::SupportDataType data) noexcept;
const ara::core::ErrorDomain& GetComDomain() noexcept;
}  // namespace com
}  // namespace ara
#endif  // ARA_COM_COM_ERROR_DOMAIN_H_
