/**
 * @file method_proxy.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/com/someip/method_proxy.h"

namespace ara {
namespace com {
namespace someip {

MethodProxy::MethodProxy(uint16_t method_id) : method_id_{method_id} {}

ara::core::Result<std::vector<uint8_t>> MethodProxy::HandleMethod(
    std::vector<uint8_t> payload) {
  return call_callback_(method_id_, payload);
}
void MethodProxy::HandleMethodAsync(std::vector<uint8_t> payload,
                                    const MethodCallback& callback_) {
  callback_(std::vector<uint8_t>{});
}
}  // namespace someip
}  // namespace com
}  // namespace ara
