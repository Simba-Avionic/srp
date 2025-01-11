/**
 * @file skeleton_connector.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_SKELETON_CONNECTOR_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_SKELETON_CONNECTOR_H_
#include <memory>
#include <thread>  // NOLINT
#include <unordered_map>
#include <string>

#include "bindings/common/socket/proccess_socket.h"
#include "ara/com/someip/someip_frame.h"
#include "platform/common/someip_demon/code/common/udp_socket.h"
#include "platform/common/someip_demon/code/connector/common_connector.h"
#include "platform/common/someip_demon/code/connector/i_db.h"

namespace srp {
namespace someip_demon {
namespace connectors {
class SkeletonConnector : public CommonConnector {
 private:
  struct ReqDetails {
    std::string ip_;
    uint16_t port_;
    uint16_t ses_id;
    std::chrono::steady_clock::time_point time_point_;
  };
  std::unique_ptr<std::jthread> time_out_thread_;
  std::unordered_map<uint32_t, ReqDetails> req_list_;
  uint32_t req_id{1};
  const std::shared_ptr<IDb> db_;
  void TimeOutLoop(std::stop_token token);
  void SendResult(const ReqDetails& det, ara::com::someip::SomeipFrame frame);

 public:
  void ProcessFrame(uint32_t pid,
                    ara::com::someip::SomeipFrame&& frame) noexcept override;
  void ProcessFrame(const std::string& ip, std::uint16_t port,
                    ara::com::someip::SomeipFrame&& frame) noexcept override;
  void Start() noexcept override;
  explicit SkeletonConnector(const std::shared_ptr<IDb> db);
  ~SkeletonConnector() = default;
};
}  // namespace connectors
}  // namespace someip_demon
}  // namespace srp
#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_SKELETON_CONNECTOR_H_
