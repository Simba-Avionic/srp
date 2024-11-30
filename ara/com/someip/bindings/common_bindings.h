/**
 * @file common_bindings.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_COM_SOMEIP_BINDINGS_COMMON_BINDINGS_H_
#define ARA_COM_SOMEIP_BINDINGS_COMMON_BINDINGS_H_

#include <cstdint>
#include <functional>
#include <vector>

#include "ara/com/someip/bindings/i_bindings.h"
#include "ara/com/someip/network_endpoint.h"
#include "ara/com/someip/someip_frame.h"
#include "ara/core/result.h"
namespace ara {
namespace com {
namespace someip {
namespace bindings {
class CommonBindings : public IBindings {
 protected:
  MethodReqWithResponse method_req_with_response_;
  MethodReqWithoutResponse method_req_without_response_;
  EventNotification event_notification_callback_;
  SubscribeNewStatus subscribe_new_status_;
  uint16_t service_id_{0U};
  uint16_t instance_id_{0U};
  std::uint8_t major_version_{0U};
  std::uint32_t minor_version_{0U};

 public:
  void SetSkeletonCallback(
      MethodReqWithResponse method_req_with_response) final {
    method_req_with_response_ = method_req_with_response;
  }
  void SetServiceData(const uint16_t& service_id,
                      const ara::core::model::ModelSomeIp& model) final {
    service_id_ = service_id;
    instance_id_ = model.instance_;
    major_version_ = model.major_version_;
    minor_version_ = model.minor_version_;
  }
  void SetProxyCallback(EventNotification event_notification_callback,
                        SubscribeNewStatus subscribe_new_status) final {
    event_notification_callback_ = event_notification_callback;
    subscribe_new_status_ = subscribe_new_status;
  }
};
}  // namespace bindings
}  // namespace someip
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_SOMEIP_BINDINGS_COMMON_BINDINGS_H_
