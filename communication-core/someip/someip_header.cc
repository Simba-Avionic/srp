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
#include "communication-core/someip/someip_header.h"

namespace simba {
namespace com {
namespace core {
namespace someip {
SomeIpHeader::SomeIpHeader(/* args */) { this->SetData(); }
SomeIpHeader::SomeIpHeader(const SomeIpHeader& other)
    : service_id_{other.service_id_.Get()},
      method_id_{other.method_id_.Get()},
      length_{other.length_.Get()},
      client_id_{other.client_id_.Get()},
      session_id_{other.session_id_.Get()},
      interface_version_{other.interface_version_.Get()},
      message_type_{other.message_type_.Get()},
      return_code_{other.return_code_.Get()} {
  this->SetData();
}
SomeIpHeader::SomeIpHeader(const uint16_t& service_id,
                           const uint16_t& method_id, const uint32_t& length,
                           const uint16_t& client_id,
                           const uint16_t& session_id,
                           const uint8_t& interface_ver,
                           const uint8_t& message_type,
                           const uint8_t& return_code)
    : service_id_{service_id},
      method_id_{method_id},
      length_{0x08 + length},
      client_id_{client_id},
      session_id_{session_id},
      interface_version_{interface_ver},
      message_type_{message_type},
      return_code_{return_code} {
  this->SetData();
}
SomeIpHeader::~SomeIpHeader() {}

void SomeIpHeader::SetData() {
  this->AddData(&service_id_);
  this->AddData(&method_id_);
  this->AddData(&length_);
  this->AddData(&client_id_);
  this->AddData(&session_id_);
  this->AddData(&protocol_version_);
  this->AddData(&interface_version_);
  this->AddData(&message_type_);
  this->AddData(&return_code_);
}
// GCOVR_EXCL_START
uint16_t SomeIpHeader::GetServiceID() const { return this->service_id_.Get(); }
uint16_t SomeIpHeader::GetMethodID() const { return this->method_id_.Get(); }
uint32_t SomeIpHeader::GetLength() const { return this->length_.Get(); }
uint16_t SomeIpHeader::GetClientID() const { return this->client_id_.Get(); }
uint16_t SomeIpHeader::GetSessionID() const { return this->session_id_.Get(); }
uint8_t SomeIpHeader::GetProtovolVersion() const {
  return this->protocol_version_.Get();
}
uint8_t SomeIpHeader::GetInterfaceVersion() const {
  return this->interface_version_.Get();
}
uint8_t SomeIpHeader::GetMessageType() const {
  return this->message_type_.Get();
}
uint8_t SomeIpHeader::GetReturnCode() const { return this->return_code_.Get(); }

void SomeIpHeader::SetServiceID(const uint16_t& value) {
  this->service_id_.Set(value);
}
void SomeIpHeader::SetMethodID(const uint16_t& value) {
  this->method_id_.Set(value);
}
void SomeIpHeader::SetLength(const uint32_t& value) {
  this->length_.Set(value + 0x08);
}
void SomeIpHeader::SetClientID(const uint16_t& value) {
  this->client_id_.Set(value);
}
void SomeIpHeader::SetSessionID(const uint16_t& value) {
  this->session_id_.Set(value);
}
void SomeIpHeader::SetSInterfaceVersion(const uint8_t& value) {
  this->interface_version_.Set(value);
}
void SomeIpHeader::SetMessageType(const uint8_t& value) {
  this->message_type_.Set(value);
}
void SomeIpHeader::SetReturnCode(const uint8_t& value) {
  this->return_code_.Set(value);
}
// GCOVR_EXCL_STOP
}  // namespace someip
}  // namespace core
}  // namespace com
}  // namespace simba
