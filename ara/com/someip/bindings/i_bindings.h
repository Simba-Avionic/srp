/**
 * @file i_bindings.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_COM_SOMEIP_BINDINGS_I_BINDINGS_H_
#define ARA_COM_SOMEIP_BINDINGS_I_BINDINGS_H_
#include <cstdint>
#include <functional>
#include <stop_token>  // NOLINT
#include <vector>

#include "ara/com/someip/network_endpoint.h"
#include "ara/com/someip/someip_frame.h"
#include "ara/core/model/com_model_item.h"
#include "ara/core/result.h"

namespace ara {
namespace com {
namespace someip {
namespace bindings {
class IBindings {
 protected:
  enum SubscribeStatus : std::uint8_t {
    kNotSubscribe = 0,
    kSubscribePending = 1,
    kSubscribe = 2
  };
  using MethodReqWithResponse =
      std::function<ara::core::Result<std::vector<uint8_t>>(
          const uint16_t& method_id, const std::vector<uint8_t>& payload)>;

  using MethodReqWithoutResponse = std::function<void(
      const uint16_t& method_id, const std::vector<uint8_t>& payload)>;

  using SubscribeNewStatus =
      std::function<void(const uint16_t event_id_, const uint8_t new_status)>;
  using EventNotification =
      std::function<void(const uint16_t, const std::vector<uint8_t>&)>;

 public:
  virtual ~IBindings() = default;
  virtual void Start(std::stop_token token) = 0;
  virtual void Stop() = 0;
  /// @brief For Proxy Request
  /// @param method_id
  /// @param payload
  /// @return
  virtual ara::core::Result<std::vector<uint8_t>> HandleMethod(
      const uint16_t& method_id, const std::vector<uint8_t>& payload) = 0;
  /// @brief For Skeleton Notyfication
  /// @param method_id
  /// @param payload
  virtual void HandleEvent(const uint16_t& method_id,
                           const std::vector<uint8_t>& payload) = 0;

  virtual void SubscribeToEvent(const uint16_t& event_id) = 0;

  /// @brief For Skeletons
  /// @param method_req_with_response
  /// @param method_req_without_response
  virtual void SetSkeletonCallback(
      MethodReqWithResponse method_req_with_response) = 0;
  virtual void SetProxyCallback(EventNotification event_notification_callback,
                                SubscribeNewStatus subscribe_new_status) = 0;
  virtual void SetServiceData(const uint16_t& service_id,
                              const ara::core::model::ModelSomeIp& model) = 0;
};
}  // namespace bindings
}  // namespace someip
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_SOMEIP_BINDINGS_I_BINDINGS_H_
