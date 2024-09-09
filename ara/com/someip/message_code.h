/**
 * @file message_code.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief Tis file define message code
 * @version 1.0
 * @date 2023-10-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef ARA_COM_SOMEIP_MESSAGE_CODE_H_
#define ARA_COM_SOMEIP_MESSAGE_CODE_H_

#include <cstdint>

namespace ara {
namespace com {
namespace someip {
enum MessageCode : uint8_t {
  kEOk = 0x00,
  kENotOk = 0x01,
  kEUnknownService = 0x02,
  kEUnknownMethod = 0x03,
  kENotReady = 0x04,
  kENotReachable = 0x05,
  kETimeout = 0x06,
  kEWrongProtocolVersion = 0x07,
  kEWrongInterfaceVersion = 0x08,
  kEMalformedMessage = 0x09
};
}  // namespace someip
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_SOMEIP_MESSAGE_CODE_H_
