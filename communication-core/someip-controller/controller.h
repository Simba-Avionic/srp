/**
 * @file controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_CONTROLLER_CONTROLLER_H_
#define COMMUNICATION_CORE_SOMEIP_CONTROLLER_CONTROLLER_H_

#include <memory>
#include <mutex>  // NOLINT
#include <string>
#include <thread>  // NOLINT
#include <unordered_map>
#include <utility>
#include <vector>

#include "communication-core/sockets/Isocket.h"
#include "communication-core/someip-controller/callbacks.h"
#include "communication-core/someip-controller/iproxy.h"
#include "communication-core/someip-controller/iskeleton.h"
#include "communication-core/someip-database/code/database.h"
#include "communication-core/someip/factory/Isomeip_header_factory.h"
#include "communication-core/someip/factory/Isomeip_message_factory.h"
#include "core/common/wait_queue.h"
namespace srp {
namespace com {
namespace someip {
class Controller {
 protected:
  std::unordered_map<uint32_t, MethodCallback> callback{};
  std::unordered_map<uint32_t, ResultCallback> event_callback{};
  std::shared_ptr<objects::DataBase> db;
  std::unique_ptr<com::soc::ISocket> udp_soc_;
  std::mutex udp_soc_mutex{};
  std::unique_ptr<com::soc::ISocket> ipc_soc_;
  std::mutex ipc_soc_mutex{};
  std::unique_ptr<factory::ISomeIpHeaderFactory> header_factory;
  std::unique_ptr<factory::ISomeIpMessageFactory> msg_factory;
  std::mutex transfer_mutex{};
  std::vector<std::pair<uint16_t, ResultCallback>> transfer_list{};
  srp::core::WaitQueue<
      std::pair<const objects::Endpoint, std::vector<uint8_t>>>
      event_queue{};
  std::unique_ptr<std::thread> event_thread;
  const uint16_t service_id;
  uint16_t transfer_id{0};
  void FindServiceCallback(const objects::Endpoint& endpoint,
                           FindServiceCallbackResult res_callback) noexcept;

  uint16_t SendFrameCallback(const std::vector<uint8_t> payload,
                             const objects::Endpoint endpoint,
                             const objects::Interface interface,
                             data::MessageType type,
                             ResultCallback result) noexcept;

  void SubscribeCallback(const objects::Endpoint endpoint,
                         const objects::Interface interface,
                         ResultCallback result) noexcept;
  void DropTransferCallback(const uint16_t transfer_id) noexcept;

  void RXCallback(const std::string& ip, const std::uint16_t& port,
                  std::vector<std::uint8_t> data) noexcept;
  bool SendIpcMsg(const objects::Interface interface,
                  const std::vector<uint8_t>& payload);
  bool SendUdpMsg(const objects::Interface interface,
                  const std::vector<uint8_t>& payload);
  bool Transfer(const objects::Interface interface,
                const std::vector<uint8_t>& payload);
  uint16_t GetTransferID();
  void SendAck(const uint16_t client_id, const uint16_t endpoint_id,
               const uint16_t trans_id);
  void SendError(std::shared_ptr<SomeIpHeader> rx_header,
                 data::MessageCode error_code);
  void onRequest(std::shared_ptr<SomeIpHeader> header,
                 const std::vector<uint8_t> payload);
  void onResult(std::shared_ptr<SomeIpHeader> header,
                const std::vector<uint8_t> payload);
  void onEvent(std::shared_ptr<SomeIpHeader> header,
               const std::vector<uint8_t> payload);
  void SendEventLoop(const objects::Endpoint endpoint,
                     std::vector<uint8_t> data);
  void SendEvent(const objects::Endpoint endpoint, const uint16_t client,
                 std::vector<uint8_t> data);
  void ThreadLoop();
  void onError(std::shared_ptr<SomeIpHeader> header);

 public:
  void Add(std::shared_ptr<IProxy> proxy);
  void Add(std::shared_ptr<ISkeleton> skeleton);
  uint16_t GetServiceId() const { return service_id; }
  explicit Controller(std::shared_ptr<objects::DataBase> db_);
  ~Controller() = default;
};

}  // namespace someip
}  // namespace com
}  // namespace srp

#endif  // COMMUNICATION_CORE_SOMEIP_CONTROLLER_CONTROLLER_H_
