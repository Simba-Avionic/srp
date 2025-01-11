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
#ifndef ARA_COM_SOMEIP_ERROR_H_
#define ARA_COM_SOMEIP_ERROR_H_
#include <string>

#include "ara/com/someip/message_code.h"
#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
namespace ara {
namespace com {
namespace someip {
class SomeIpErrorDomain final : public ara::core::ErrorDomain {
 public:
  using Errc = someip::MessageCode;

  SomeIpErrorDomain() noexcept;
  const std::string Name() const noexcept override;
  const std::string Message(
      ara::core::ErrorDomain::CodeType errorCode) const noexcept override;
};

ara::core::ErrorCode MakeErrorCode(
    SomeIpErrorDomain::Errc code,
    ara::core::ErrorDomain::SupportDataType data) noexcept;
const ara::core::ErrorDomain& GetComDomain() noexcept;
}  // namespace someip
}  // namespace com
}  // namespace ara
#endif  // ARA_COM_SOMEIP_ERROR_H_
