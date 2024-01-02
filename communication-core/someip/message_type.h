/**
 * @file message_type.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file define message types
 * @version 1.0
 * @date 2023-10-21
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_MESSAGE_TYPE_H_
#define COMMUNICATION_CORE_SOMEIP_MESSAGE_TYPE_H_
namespace simba {
namespace com {
namespace core {
namespace data {
enum MessageType {
  kRequest = 0x00,
  kRequestNoReturn = 0x01,
  kNotification = 0x02,
  kRequestAck = 0x40,
  kRequestNoReturnAck = 0x41,
  kNotificationAck = 0x42,
  kResponse = 0x80,
  kError = 0x81,
  kResponseAck = 0xC0,
  kErrorAck = 0xC1
};
}  // namespace data
}  // namespace core
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_MESSAGE_TYPE_H_
