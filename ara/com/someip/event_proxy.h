/**
 * @file event_proxy.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_COM_SOMEIP_EVENT_PROXY_H_
#define ARA_COM_SOMEIP_EVENT_PROXY_H_

#include <cstdint>
#include <functional>
#include <vector>

#include "ara/core/result.h"

namespace ara {
namespace com {
namespace someip {
class EventProxy {
 public:
  enum SubscribeStatus : std::uint8_t {
    kNotSubscribe = 0,
    kSubscribePending = 1,
    kSubscribe = 2
  };
  using SubscriptionStateChangeHandler =
      std::function<void(const std::uint8_t staus)>;
  using EventReceiveHandler = std::function<void()>;
  using SubscribeCallback = std::function<void(const uint16_t&)>;

 protected:
  const uint16_t event_id_;
  bool msg_recived{false};
  SubscribeStatus status_{SubscribeStatus::kNotSubscribe};
  EventReceiveHandler event_receive_handler_;
  SubscriptionStateChangeHandler subscription_state_change_handler_;
  SubscribeCallback subscribe_callback_;

 public:
  explicit EventProxy(uint16_t event_id);

  virtual void HandleEvent(const std::vector<uint8_t>& payload) = 0;
  void _SetSubscriptionState(const uint8_t new_state);
  void _SetSubscriptionCallback(SubscribeCallback subscribe_callback);
  ara::core::Result<void> Subscribe(size_t maxSampleCount);
  ara::core::Result<void> Subscribe(size_t maxSampleCount,
                                    SubscriptionStateChangeHandler handler);
  ara::core::Result<void> SetSubscriptionStateChangeHandler(
      SubscriptionStateChangeHandler handler);
  ara::core::Result<void> SetReceiveHandler(EventReceiveHandler handler);
  virtual ~EventProxy() = default;
};
}  // namespace someip

}  // namespace com

}  // namespace ara

#endif  // ARA_COM_SOMEIP_EVENT_PROXY_H_
