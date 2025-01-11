/**
 * @file proxy_connector.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_PROXY_CONNECTOR_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_PROXY_CONNECTOR_H_
#include <memory>
#include <string>
#include <thread>  // NOLINT
#include <unordered_map>
#include <utility>
#include <vector>

#include "bindings/common/socket/proccess_socket.h"
#include "ara/com/someip/someip_frame.h"
#include "platform/common/someip_demon/code/common/udp_socket.h"
#include "platform/common/someip_demon/code/connector/common_connector.h"
#include "platform/common/someip_demon/code/connector/i_db.h"

namespace srp {
namespace someip_demon {
namespace connectors {
class ProxyConnector : public CommonConnector {
 private:
  struct ReqDetails {
    uint16_t ses_id;
    uint16_t service_id;
    uint16_t method_id;
    uint32_t process_id;
    std::chrono::steady_clock::time_point time_point_;
  };
  std::unique_ptr<std::jthread> time_out_thread_;
  std::unordered_map<uint32_t, ReqDetails> req_list_;
  std::unordered_map<uint64_t,
                     std::reference_wrapper<const db::FindServiceItem>>
      cache_{};
  uint16_t req_id{1};
  const std::shared_ptr<IDb> db_;
  void TimeOutLoop(std::stop_token token);
  void SendResult(const ReqDetails& det, ara::com::someip::SomeipFrame frame);

 public:
  void ProcessFrame(uint32_t pid,
                    ara::com::someip::SomeipFrame&& frame) noexcept override;
  void ProcessFrame(const std::string& ip, std::uint16_t port,
                    ara::com::someip::SomeipFrame&& frame) noexcept override;
  void Start() noexcept override;
  explicit ProxyConnector(const std::shared_ptr<IDb> db);
  ~ProxyConnector() = default;
};
}  // namespace connectors
}  // namespace someip_demon
}  // namespace srp
#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_PROXY_CONNECTOR_H_
