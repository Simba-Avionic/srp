/**
 * @file method_proxy.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_COM_SOMEIP_METHOD_PROXY_H_
#define ARA_COM_SOMEIP_METHOD_PROXY_H_

#include <cstdint>
#include <functional>
#include <vector>

#include "ara/com/someip/network_endpoint.h"
#include "ara/com/someip/someip_frame.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"

namespace ara {
namespace com {
namespace someip {
class MethodProxy {
 public:
  using MethodCallback =
      std::function<void(ara::core::Result<std::vector<uint8_t>>)>;
  using CallCallback = std::function<ara::core::Result<std::vector<uint8_t>>(
      const uint16_t& method_id, const std::vector<uint8_t>& data)>;
  explicit MethodProxy(uint16_t method_id);
  ~MethodProxy() = default;
  void _SetCallCallback(CallCallback callback) { call_callback_ = callback; }

 private:
  const uint16_t method_id_;
  CallCallback call_callback_;

 protected:
  ara::core::Result<std::vector<uint8_t>> HandleMethod(
      std::vector<uint8_t> payload);
  void HandleMethodAsync(std::vector<uint8_t> payload,
                         const MethodCallback& callback_);
};

}  // namespace someip
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_SOMEIP_METHOD_PROXY_H_
