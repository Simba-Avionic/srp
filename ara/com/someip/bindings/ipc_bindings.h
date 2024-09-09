/**
 * @file ipc_bindings.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_COM_SOMEIP_BINDINGS_IPC_BINDINGS_H_
#define ARA_COM_SOMEIP_BINDINGS_IPC_BINDINGS_H_
#include <cstdint>
#include <functional>
#include <memory>
#include <stop_token> // NOLINT
#include <thread>  // NOLINT
#include <vector>

#include "ara/com/shm/shm_buffor_proxy.h"
#include "ara/com/shm/shm_buffor_skeleton.h"
#include "ara/com/someip/bindings/common_bindings.h"
#include "ara/com/someip/network_endpoint.h"
#include "ara/com/someip/someip_frame.h"
#include "ara/core/condition_variable.h"
#include "ara/core/result.h"
#include "communication-core/sockets/stream_ipc_socket.h"
#include "core/common/condition.h"
namespace ara {
namespace com {
namespace someip {
namespace bindings {
class IPCSkeletonBindings : public CommonBindings {
 protected:
  void SocketLoop();
  const ara::core::StringView main_path_;
  ara::com::shm::ShmBufforSkeleton<1024> buffor_;
  ara::core::ConditionVariableSkeleton cv_;

 public:
  explicit IPCSkeletonBindings(const ara::core::StringView path);
  void Start(std::stop_token token) override;
  void Stop() override;
  ara::core::Result<std::vector<uint8_t>> HandleMethod(
      const uint16_t& method_id, const std::vector<uint8_t>& payload) override;
  void HandleEvent(const uint16_t& method_id,
                   const std::vector<uint8_t>& payload) override;
  void SubscribeToEvent(const uint16_t& event_id) override;
};

class IPCProxyBindings : public CommonBindings {
 protected:
  const uint8_t last_msg_id{0U};
  void ShmLoop(std::stop_token token);
  ara::com::shm::ShmBufforProxy<1024> buffor_;
  ara::core::ConditionVariableProxy cv_;
  std::unique_ptr<std::jthread> shm_loop_thread_;

 public:
  explicit IPCProxyBindings(const ara::core::StringView path);
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

#endif  // ARA_COM_SOMEIP_BINDINGS_IPC_BINDINGS_H_
