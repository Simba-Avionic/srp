/**
 * @file dtc_header.cpp
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "mw/diag/dtc/msg/dtc_header.hpp"

namespace simba {
namespace mw {
namespace dtc {

DtcHeader::DtcHeader(const uint16_t& service_id, const uint16_t& method_id)
    : method_id_(method_id), service_id_(service_id) {
  this->lenght_ = 0x06;
}
DtcHeader::DtcHeader() {}

uint16_t DtcHeader::GetServiceID() { return this->service_id_; }
uint16_t DtcHeader::GetMethodID() { return this->method_id_; }
uint16_t DtcHeader::GetLength() { return this->lenght_; }

void DtcHeader::SetServiceID(const uint16_t& value) {
  this->service_id_ = value;
}
void DtcHeader::SetMethodID(const uint16_t& value) { this->method_id_ = value; }
void DtcHeader::SetLength(const uint16_t& value) {
  this->lenght_ = value + 0x06;
}

}  // namespace dtc
}  // namespace mw
}  // namespace simba
