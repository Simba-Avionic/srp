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

namespace srp {
namespace mw {
namespace temp {

SubscribeHeader::SubscribeHeader(const uint16_t &service_id)
    : service_id_(service_id) {
    SetData();
}

SubscribeHeader::SubscribeHeader()
    : service_id_(0U) {
    SetData();
}

uint16_t SubscribeHeader::GetServiceID() const {
    return this->service_id_.Get();
}


void SubscribeHeader::SetData() {
    this->AddData(&service_id_);
}

}  // namespace temp
}  // namespace mw
}  // namespace srp
