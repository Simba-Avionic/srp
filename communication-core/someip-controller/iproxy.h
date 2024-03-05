/**
 * @file iproxy.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_CONTROLLER_IPROXY_H_
#define COMMUNICATION_CORE_SOMEIP_CONTROLLER_IPROXY_H_

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "communication-core/someip-controller/callbacks.h"
#include "communication-core/someip-database/code/endpoint.h"
#include "communication-core/someip-database/code/interface.h"

namespace simba {
namespace com {
namespace someip {
class IProxy {
 public:
  virtual void SetSendCallback(SendCallback callback, FindCallback f_callback,
                               DropTransferCallback d_callback,
                               SubscribeCallback s_callback) = 0;
  virtual std::string GetName() const = 0;
  virtual std::optional<objects::Endpoint> GetEndPoint() const noexcept = 0;
  virtual ~IProxy() = default;
};
}  // namespace someip
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_CONTROLLER_IPROXY_H_
