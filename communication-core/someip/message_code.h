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
#ifndef COMMUNICATION_CORE_SOMEIP_MESSAGE_CODE_H_
#define COMMUNICATION_CORE_SOMEIP_MESSAGE_CODE_H_
namespace simba {
namespace com {
namespace core {
namespace data {
enum MessageCode {
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
}  // namespace data
}  // namespace core
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_MESSAGE_CODE_H_
