/**
 * @file someip_header.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file define someip header as network structure
 * @version 1.0
 * @date 2023-08-27
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_SOMEIP_HEADER_H_
#define COMMUNICATION_CORE_SOMEIP_SOMEIP_HEADER_H_
#include "communication-core/network-data/network_data_structure.h"
#include "communication-core/network-data/network_data_type.h"

namespace simba {
namespace com {
namespace core {
namespace someip {
class SomeIpHeader : public core::network::NetworkDataStructure {
 private:
  core::network::uint16_t service_id_;
  core::network::uint16_t method_id_;
  core::network::uint32_t length_{0x08};
  core::network::uint16_t client_id_;
  core::network::uint16_t session_id_;
  core::network::uint8_t protocol_version_{0x01};
  core::network::uint8_t interface_version_;
  core::network::uint8_t message_type_;
  core::network::uint8_t return_code_;
  void SetData();

 public:
  uint16_t GetServiceID() const;
  uint16_t GetMethodID() const;
  uint32_t GetLength() const;
  uint16_t GetClientID() const;
  uint16_t GetSessionID() const;
  uint8_t GetProtovolVersion() const;
  uint8_t GetInterfaceVersion() const;
  uint8_t GetMessageType() const;
  uint8_t GetReturnCode() const;

  void SetServiceID(const uint16_t& value);
  void SetMethodID(const uint16_t& value);
  void SetLength(const uint32_t& value);
  void SetClientID(const uint16_t& value);
  void SetSessionID(const uint16_t& value);
  void SetSInterfaceVersion(const uint8_t& value);
  void SetMessageType(const uint8_t& value);
  void SetReturnCode(const uint8_t& value);

  SomeIpHeader(const uint16_t& service_id, const uint16_t& method_id,
               const uint32_t& length, const uint16_t& client_id,
               const uint16_t& session_id, const uint8_t& interface_ver,
               const uint8_t& message_type, const uint8_t& return_code);
  SomeIpHeader();
  SomeIpHeader(const SomeIpHeader& other);
  ~SomeIpHeader();
};

}  // namespace someip
}  // namespace core
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_SOMEIP_HEADER_H_
