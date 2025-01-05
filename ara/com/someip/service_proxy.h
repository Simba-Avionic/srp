/**
 * @file service_proxy.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_COM_SOMEIP_SERVICE_PROXY_H_
#define ARA_COM_SOMEIP_SERVICE_PROXY_H_

#include <functional>
#include <memory>
#include <stop_token>  // NOLINT
#include <thread>      // NOLINT

#include "ara/core/instance_specifier.h"

namespace ara {
namespace com {
namespace someip {
template <typename ServiceHandler_t>
class ServiceProxy {
 public:
  using FindCallback = std::function<void(std::shared_ptr<ServiceHandler_t>)>;
  using handler_t = std::shared_ptr<ServiceHandler_t>;

 protected:
  const ara::core::InstanceSpecifier instance_;
  FindCallback find_callback_;
  std::unique_ptr<std::jthread> find_thread_;
  std::shared_ptr<ServiceHandler_t> handler_;
  void find_loop(std::stop_token token) {
    if (handler_ == nullptr) {
      handler_ = std::make_shared<ServiceHandler_t>(instance_);

      // TODO(Bartosz Snieg): need to be fixed
      std::ignore = handler_->StartFind(token);
    }
    if (!token.stop_requested()) {
      find_callback_(handler_);
    }
  }

 public:
  explicit ServiceProxy(const ara::core::InstanceSpecifier& instance)
      : instance_{instance} {}
  void StartFindService(FindCallback find_callback) {
    this->find_callback_ = find_callback;
    this->find_thread_ = std::make_unique<std::jthread>(
        [this](std::stop_token token) { this->find_loop(token); });
  }
  void StopFindService() {
    if (find_thread_ != nullptr) {
      find_thread_->request_stop();
      find_thread_->join();
    }
  }
};
}  // namespace someip
}  // namespace com
}  // namespace ara
#endif  // ARA_COM_SOMEIP_SERVICE_PROXY_H_
