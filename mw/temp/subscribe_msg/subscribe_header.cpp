/**
 * @file 
 * @author 
 * @brief 
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "subscribe_header.h"

namespace simba {
namespace mw {
namespace temp {

SubscribeHeader::SubscribeHeader(const uint16_t &service_id)
    : service_id_(service_id), length_(sizeof(uint16_t)) {
    SetData();
}

SubscribeHeader::SubscribeHeader()
    : service_id_(0U), length_(sizeof(uint16_t)) {
    SetData();
}

uint16_t SubscribeHeader::GetServiceID() const {
    return this->service_id_.Get();
}

uint8_t SubscribeHeader::GetLength() const {
    return this->length_.Get();
}

void SubscribeHeader::SetLength(const uint8_t& value) {
    this->length_ = value + sizeof(uint16_t);
}

void SubscribeHeader::SetData() {
    this->AddData(&service_id_);
    this->AddData(&length_);
}

}  // namespace temp
}  // namespace mw
}  // namespace simba
