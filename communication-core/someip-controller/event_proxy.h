/**
 * @file event_proxy.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-03-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef COMMUNICATION_CORE_SOMEIP_CONTROLLER_EVENT_PROXY_H_
#define COMMUNICATION_CORE_SOMEIP_CONTROLLER_EVENT_PROXY_H_

#include <condition_variable>  // NOLINT
#include <mutex>               // NOLINT
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "communication-core/someip-controller/iproxy.h"
#include "communication-core/someip-database/code/com_config.h"
#include "communication-core/someip-database/code/endpoint.h"
#include "communication-core/someip-database/code/interface.h"
#include "communication-core/someip/message_code.h"
#include "communication-core/someip/message_type.h"
#include "core/logger/Logger.h"

namespace srp {
namespace com {
namespace someip {

using EventCallback = std::function<void(const std::vector<uint8_t> payload)>;

class EventProxyBase : public IProxy {
 private:
  const std::string instance;
  const std::optional<objects::Endpoint> object;
  std::optional<objects::Interface> interface;
  std::mutex msg_mutex{};
  std::condition_variable request_cv{};
  bool is_callback{false};
  bool is_response{false};
  std::vector<uint8_t> response;
  com::data::MessageCode response_code;
  FindCallback f_callback_;
  SubscribeCallback s_callback;
  EventCallback callback_;
  const bool is_event_callback;
  void RxCallback(const std::vector<uint8_t> payload,
                  srp::com::data::MessageCode code, const uint16_t) {
    if (code == com::data::MessageCode::kEOk) {
      if (is_event_callback) {
        callback_(std::move(payload));
        return;
      } else {
        response = std::move(payload);
        is_response = true;
        request_cv.notify_all();
      }
    } else {
      AppLogger::Error("[ SOMEIP PROXY ]: Received frame with wrong data type");
    }
  }
  void FindServiceCallback(const objects::Interface& interface) {
    AppLogger::Info("[ SOMEIP PROXY ]: Service fonded (" + instance + ") - " +
                    interface.GetIp() + ":" +
                    std::to_string(interface.GetPort()));
    this->interface = std::move(interface);
    this->s_callback(
        object.value(), this->interface.value(),
        std::bind(&EventProxyBase::RxCallback, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));
  }

 public:
  explicit EventProxyBase(const std::string& instance_)
      : instance{instance_},
        object{std::move(com::config::ComConfig::FindObject(instance))},
        is_event_callback{false} {
    if (!object.has_value()) {
      AppLogger::Error("[ SOMEIP PROXY ]: Can't find object for: " + instance);
    } else {
      AppLogger::Info("[ SOMEIP PROXY ]: " + instance + " mapped to " +
                      std::to_string(object.value().GetServiceId()) + "/" +
                      std::to_string(object.value().GetEndpointId()));
    }
  }
  EventProxyBase(const std::string& instance_, EventCallback callback)
      : instance{instance_},
        object{std::move(com::config::ComConfig::FindObject(instance))},
        callback_{callback},
        is_event_callback{true} {
    if (!object.has_value()) {
      AppLogger::Error("[ SOMEIP PROXY ]: Can't find object for: " + instance);
    } else {
      AppLogger::Info("[ SOMEIP PROXY ]: " + instance + " mapped to " +
                      std::to_string(object.value().GetServiceId()) + "/" +
                      std::to_string(object.value().GetEndpointId()));
    }
  }
  std::optional<objects::Endpoint> GetEndPoint() const noexcept override {
    return this->object;
  }

  std::optional<std::vector<uint8_t>> Get() noexcept {
    if (is_response) {
      is_response = false;
      return response;
    } else {
      std::unique_lock lk(msg_mutex);
      if (request_cv.wait_for(lk, std::chrono::seconds{1}) ==
          std::cv_status::timeout) {
        AppLogger::Error("[ SOMEIP PROXY ](" + instance + "): Timeout !");
        return {};
      }
      is_response = false;
      return response;
    }
  }

  void SetSendCallback(SendCallback, FindCallback f_callback,
                       DropTransferCallback, SubscribeCallback sub_callback) {
    f_callback_ = f_callback;
    is_callback = true;
    s_callback = sub_callback;
  }
  void StartFindService() {
    if (is_callback && object.has_value()) {
      f_callback_(object.value(),
                  std::bind(&EventProxyBase::FindServiceCallback, this,
                            std::placeholders::_1));
    }
  }
  std::string GetName() const { return instance; }

  ~EventProxyBase() = default;
};
}  // namespace someip
}  // namespace com
}  // namespace srp
#endif  // COMMUNICATION_CORE_SOMEIP_CONTROLLER_EVENT_PROXY_H_
