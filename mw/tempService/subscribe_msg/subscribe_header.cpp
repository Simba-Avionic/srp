/**
 * @file exec_header.cpp
 * @author Wiktor Kawka (asinesak353@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "subscribe_header.h"

namespace simba {
namespace mw {
namespace temp {

SubscribeHeader::SubscribeHeader(const uint16_t &service_id)
        : service_id_(service_id){}

SubscribeHeader::SubscribeHeader() 
        : service_id_(0){}

uint16_t SubscribeHeader::GetServiceID() const {
    return this->service_id_;
}


}  // namespace temp
}  // namespace mw
}  // namespace simba
