/**
 * @file udp_proxy_bindings.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_COM_SOMEIP_BINDINGS_PROXY_UDP_PROXY_BINDINGS_H_
#define ARA_COM_SOMEIP_BINDINGS_PROXY_UDP_PROXY_BINDINGS_H_

#include <atomic>
#include <condition_variable>  // NOLINT
#include <memory>
#include <mutex>  // NOLINT
#include <unordered_map>
#include <vector>

#include "ara/com/someip/bindings/common_bindings.h"
#include "ara/com/someip/controller/i_someip_controller.h"

namespace ara {
namespace com {
namespace someip {
namespace bindings {

class UdpProxyBindings : public CommonBindings {
 public:
  struct RequestTableItem {
    explicit RequestTableItem(const uint16_t req_id) : req_id_{req_id} {}
    const uint16_t req_id_;
    std::mutex block_mutex{};
    std::condition_variable cv_{};
    std::vector<uint8_t> payload{};
    std::uint8_t status_{0xFF};
  };

 private:
  const std::uint32_t port_;
  const std::shared_ptr<ISomeipController> controller_;
  std::condition_variable find_cv_{};
  std::mutex find_mutex_{};
  bool find_status_flag_{false};
  bool shoot_down{false};
  const uint16_t pid_{0U};
  uint16_t req_id{0U};
  ara::core::Result<void> RxCallback(
      uint32_t pid, const ara::com::someip::SomeipFrame& frame);
  std::unordered_map<uint16_t, std::shared_ptr<RequestTableItem>> req_list_{};

 public:
  explicit UdpProxyBindings(std::uint32_t port);
  void Start(std::stop_token token) override;
  void Stop() override;
  ara::core::Result<std::vector<uint8_t>> HandleMethod(
      const uint16_t& method_id, const std::vector<uint8_t>& payload) override;
  void HandleEvent(const uint16_t& method_id,
                   const std::vector<uint8_t>& payload) override;
  void SubscribeToEvent(const uint16_t& event_id) override;
};

}  // namespace bindings
}  // namespace someip
}  // namespace com
}  // namespace ara
#endif  // ARA_COM_SOMEIP_BINDINGS_PROXY_UDP_PROXY_BINDINGS_H_
