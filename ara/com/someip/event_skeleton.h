/**
 * @file event_skeleton.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_COM_SOMEIP_EVENT_SKELETON_H_
#define ARA_COM_SOMEIP_EVENT_SKELETON_H_

#include <cstdint>
#include <functional>
#include <vector>
namespace ara {
namespace com {
namespace someip {
class EventSkeleton {
 public:
  using SendCallback = std::function<void(const uint16_t event_id,
                                          const std::vector<uint8_t>& data)>;

 protected:
  const uint16_t event_id_;
  SendCallback send_callback_;

 public:
  explicit EventSkeleton(uint16_t event_id) : event_id_{event_id} {}

  void SetCallback(SendCallback send_callback) {
    send_callback_ = send_callback;
  }
  void HandleEvent(const std::vector<uint8_t>& payload) {
    this->send_callback_(event_id_, payload);
  }
  virtual ~EventSkeleton() = default;
};
}  // namespace someip

}  // namespace com

}  // namespace ara

#endif  // ARA_COM_SOMEIP_EVENT_SKELETON_H_
