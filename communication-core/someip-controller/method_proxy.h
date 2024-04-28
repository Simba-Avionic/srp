/**
 * @file method_proxy.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_CONTROLLER_METHOD_PROXY_H_
#define COMMUNICATION_CORE_SOMEIP_CONTROLLER_METHOD_PROXY_H_

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
namespace simba {
namespace com {
namespace someip {
class MethodProxyBase : public IProxy {
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
  uint16_t transfer_id_;
  SendCallback callback_;
  FindCallback f_callback_;
  DropTransferCallback d_callback_;

  void RxCallback(const std::vector<uint8_t> payload,
                  simba::com::data::MessageCode code,
                  const uint16_t transfer_id) {
    if (transfer_id == transfer_id_) {
      is_response = true;
      response_code = code;
      response = payload;
      request_cv.notify_all();
    }
  }

  void FindServiceCallback(const objects::Interface& interface) {
    AppLogger::Info("[ SOMEIP PROXY ]: Service fonded (" + instance + ") - " +
                    interface.GetIp() + ":" +
                    std::to_string(interface.GetPort()));
    this->interface = interface;
  }

 public:
  explicit MethodProxyBase(const std::string& instance_)
      : instance{instance_},
        object{std::move(com::config::ComConfig::FindObject(instance))} {
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

  std::optional<std::vector<uint8_t>> Get(std::vector<uint8_t> data) noexcept {
    if (!is_callback) {
      AppLogger::Error("[ SOMEIP PROXY ]: No send callback");
      return {};
    }
    is_response = false;
    if (!object.has_value()) {
      AppLogger::Error("[ SOMEIP PROXY ]: No object");
      return {};
    }
    if (!interface.has_value()) {
      AppLogger::Error("[ SOMEIP PROXY ]: No interface");
      return {};
    }
    std::unique_lock lk(msg_mutex);
    transfer_id_ = callback_(
        std::move(data), object.value(), interface.value(),
        simba::com::data::MessageType::kRequest,
        std::bind(&MethodProxyBase::RxCallback, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));
    if (transfer_id_ == 0) {
      AppLogger::Error("[ SOMEIP PROXY ]: Sending error");
      return {};
    }
    if (request_cv.wait_for(lk, std::chrono::seconds{1}) ==
        std::cv_status::timeout) {
      AppLogger::Error("[ SOMEIP PROXY ](" + instance + "): Timeout !");
      d_callback_(transfer_id_);
      return {};
    }
    if (response_code == com::data::MessageCode::kEOk) {
      return std::move(this->response);
    } else {
      if (response_code == com::data::MessageCode::kENotReachable) {
        AppLogger::Error("[ SOMEIP PROXY ](" + instance + "): Access denied !");
      } else {
        AppLogger::Error(
            "[ SOMEIP PROXY ](" + instance +
            "): Response with error: " + std::to_string(response_code));
      }
      return {};
    }
  }

  std::optional<std::vector<uint8_t>> Get() noexcept { return Get({}); }

  void SetSendCallback(SendCallback callback, FindCallback f_callback,
                       DropTransferCallback d_callback, SubscribeCallback) {
    callback_ = callback;
    f_callback_ = f_callback;
    d_callback_ = d_callback;
    is_callback = true;
  }
  void StartFindService() {
    if (is_callback && object.has_value()) {
      f_callback_(object.value(),
                  std::bind(&MethodProxyBase::FindServiceCallback, this,
                            std::placeholders::_1));
    }
  }
  std::string GetName() const { return instance; }

  ~MethodProxyBase() = default;
};
}  // namespace someip
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_CONTROLLER_METHOD_PROXY_H_
