/**
 * @file event_proxy.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ara/com/someip/event_proxy.h"

#include "ara/com/log.h"

namespace ara {
namespace com {
namespace someip {

EventProxy::EventProxy(uint16_t event_id) : event_id_{event_id} {}

void EventProxy::_SetSubscriptionState(const uint8_t new_state) {
  ara::com::LogInfo() << "Event id: " << this->event_id_
                      << " new state: " << new_state;
  this->status_ = static_cast<SubscribeStatus>(new_state);
  if (this->subscription_state_change_handler_) {
    this->subscription_state_change_handler_(new_state);
  }
}
void EventProxy::_SetSubscriptionCallback(
    SubscribeCallback subscribe_callback) {
  subscribe_callback_ = subscribe_callback;
}
ara::core::Result<void> EventProxy::Subscribe(size_t maxSampleCount) {
  this->subscribe_callback_(event_id_);
  return {};
}

ara::core::Result<void> EventProxy::Subscribe(
    size_t maxSampleCount, SubscriptionStateChangeHandler handler) {
  this->subscription_state_change_handler_ = handler;
  this->subscribe_callback_(event_id_);
  return {};
}

ara::core::Result<void> EventProxy::SetSubscriptionStateChangeHandler(
    SubscriptionStateChangeHandler handler) {
  this->subscription_state_change_handler_ = handler;
  return {};
}

ara::core::Result<void> EventProxy::SetReceiveHandler(
    EventReceiveHandler handler) {
  this->event_receive_handler_ = handler;
  return {};
}
}  // namespace someip
}  // namespace com
}  // namespace ara
