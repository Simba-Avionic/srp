/**
 * @file someip_controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_CONTROLLER_SOMEIP_CONTROLLER_H_
#define COMMUNICATION_CORE_SOMEIP_CONTROLLER_SOMEIP_CONTROLLER_H_
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "core/common/error_code.h"
#include "communication-core/database/database.h"
#include "core/logger/Logger.h"
#include "memory"
#include "core/results/result.h"
#include "communication-core/sockets/Isocket.h"
#include "communication-core/sockets/socket_config.h"
#include "communication-core/someip-controller/Isomeip_controller.h"
#include "communication-core/someip-controller/transfer.h"
#include "communication-core/someip/factory/someip_header_factory.h"
#include "communication-core/someip/factory/someip_message_factory.h"

namespace simba {
namespace com {
namespace someip {
class SomeIpController : public ISomeIpController {
 private:
  void RxCallback(const std::string& ip, const std::uint16_t& port,
                  std::vector<std::uint8_t> payload);
  std::unique_ptr<soc::ISocket> socket_;
  std::shared_ptr<simba::core::logger::ILogger> logger_;
  const std::uint16_t service_id_;
  const soc::SocketConfig config_;
  database::Database db_{};
  com::core::someip::factory::SomeIpHeaderFactory header_factory{};
  com::core::someip::factory::SomeIpMessageFactory msg_factory{};
  uint16_t transfer_id = 2;

  inline const uint16_t GetTransferID() { return transfer_id++; }

  std::vector<std::shared_ptr<data::Transfer>> transfers{};
  std::unordered_map<uint16_t, SomeIPMethod> methods{};
  std::shared_ptr<data::Transfer> AddTransfer(const uint16_t transfer) {
    auto res = std::make_shared<data::Transfer>(transfer);
    this->transfers.push_back(res);
    return res;
  }

  void MethodCalled(
      const std::shared_ptr<simba::com::core::someip::SomeIpHeader> header,
      std::vector<std::uint8_t> data);
  void Response(
      const std::shared_ptr<simba::com::core::someip::SomeIpHeader> header,
      std::vector<std::uint8_t> data);
  void UknowReqeust(
      const std::shared_ptr<simba::com::core::someip::SomeIpHeader> header,
      std::vector<std::uint8_t> data);
  void SendResponse(
      const std::shared_ptr<simba::com::core::someip::SomeIpHeader> header,
      std::vector<std::uint8_t> data);

 public:
  simba::core::Result<std::vector<uint8_t>> Request(
      const uint16_t service_id, const uint16_t method_id,
      const std::vector<uint8_t> payload) override;

  bool RequestNoResponse(const uint16_t service_id, const uint16_t method_id,
                         const std::vector<uint8_t> payload) override;

  simba::core::ErrorCode AddMethod(const uint16_t method_id,
                                   SomeIPMethod callback) override;

  simba::core::ErrorCode AddEventValue(
      const uint16_t event_id, const std::vector<uint8_t> payload) override;
  simba::core::ErrorCode Init() override;
  simba::core::ErrorCode LoadServiceList(const std::string& path) override;
  SomeIpController(const uint16_t service_id,
                   std::unique_ptr<soc::ISocket> socket,
                   const soc::SocketConfig& config);
};
}  // namespace someip
}  // namespace com
}  // namespace simba

#endif  // COMMUNICATION_CORE_SOMEIP_CONTROLLER_SOMEIP_CONTROLLER_H_
