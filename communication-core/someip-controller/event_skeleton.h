/**
 * @file event_skeleton.h
 * @author Bartosz Snieg (snieg45@gmial.com)
 * @brief
 * @version 0.1
 * @date 2024-03-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_CONTROLLER_EVENT_SKELETON_H_
#define COMMUNICATION_CORE_SOMEIP_CONTROLLER_EVENT_SKELETON_H_

#include <algorithm>
#include <condition_variable>  // NOLINT
#include <memory>
#include <mutex>  // NOLINT
#include <optional>
#include <string>
#include <thread>  // NOLINT
#include <utility>
#include <vector>

#include "communication-core/someip-controller/callbacks.h"
#include "communication-core/someip-controller/iskeleton.h"
#include "communication-core/someip-database/code/com_config.h"
#include "communication-core/someip-database/code/endpoint.h"
#include "communication-core/someip-database/code/interface.h"
#include "communication-core/someip/message_code.h"
#include "communication-core/someip/message_type.h"
#include "core/logger/Logger.h"

namespace simba {
namespace com {
namespace someip {
class EventSkeleton : public ISkeleton {
 private:
  const std::optional<objects::Endpoint> object;
  const std::string instance_;
  SendCallback callback_;
  std::vector<uint8_t> value{};
  std::mutex value_mutex{};

  std::mutex loop_mutex{};
  std::condition_variable loop_cv{};
  std::unique_ptr<std::jthread> thread;

 public:
  std::optional<objects::Endpoint> GetEndPoint() const noexcept override {
    return object;
  }
  std::pair<data::MessageCode, std::optional<std::vector<uint8_t>>> Call(
      std::vector<uint8_t> payload,
      const objects::Endpoint endpoint) noexcept override {
    return {};
  }
  std::string GetName() const override { return instance_; }

  explicit EventSkeleton(const std::string instance)
      : object{std::move(com::config::ComConfig::FindObject(instance))},
        instance_{std::move(instance)} {
    if (!object.has_value()) {
      AppLogger::Error("[ SOMEIP SKELETON  ]: Can't find object for: " +
                       instance);
    } else {
      AppLogger::Info("[ SOMEIP SKELETON ]: " + instance_ + " mapped to " +
                      std::to_string(object.value().GetServiceId()) + "/" +
                      std::to_string(object.value().GetEndpointId()));
    }
  }
  void Loop(std::stop_token token) {
    while (!token.stop_requested()) {
      std::unique_lock lk{loop_mutex};
      loop_cv.wait_for(lk, std::chrono::seconds{1});
      {
        std::unique_lock lkv{value_mutex};
        if (value.size() > 0) {
          callback_(this->value, object.value(), objects::Interface{"", 0},
                    data::MessageType::kNotification, nullptr);
        }
      }
    }
  }
  void SetValue(const std::vector<uint8_t>& data) {
    {
      std::unique_lock lk{value_mutex};
      value = std::move(data);
    }
    loop_cv.notify_all();
  }
  void SetCallback(SendCallback callback) {
    callback_ = callback;
    this->thread = std::make_unique<std::jthread>(
        std::bind(&EventSkeleton::Loop, this, std::placeholders::_1));
  }
  ~EventSkeleton() {
    thread->request_stop();
    loop_cv.notify_all();
  }
};
}  // namespace someip
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_CONTROLLER_EVENT_SKELETON_H_
