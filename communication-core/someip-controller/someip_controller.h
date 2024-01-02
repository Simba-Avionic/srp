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
#ifndef COMUNICATION_CORE_SOMEIP_CONTROLLER_SOMEIP_CONTROLLER_H_
#define COMUNICATION_CORE_SOMEIP_CONTROLLER_SOMEIP_CONTROLLER_H_
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>
#include <string>

#include "common/error_code.h"
#include "database/database.h"
#include "database/event_element.h"
#include "logger/ILogger.h"
#include "memory"
#include "results/result.h"
#include "sockets/Isocket.h"
#include "sockets/socket_config.h"
#include "someip-controller/Isomeip_controller.h"
#include "someip-controller/transfer.h"
#include "someip/factory/someip_header_factory.h"
#include "someip/factory/someip_message_factory.h"

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
  /**
   * @brief zmienna przechowująca baze eventów publikowanych przez Klase,
   * wraz z listą subskrybenow
   */
  std::unordered_map<uint16_t,
                    simba::database::objects::EventElement> event_db{};
  com::core::someip::factory::SomeIpHeaderFactory header_factory{};
  com::core::someip::factory::SomeIpMessageFactory msg_factory{};
  uint16_t transfer_id = 2;

  inline const uint16_t GetTransferID() { return transfer_id++; }

  std::vector<std::shared_ptr<data::Transfer>> transfers{};
  /**
   * @brief zmienna przechowująca Subskrybowane eventy i ich callbacki.
   * klucz to service_id+event_id
   */
  std::unordered_map<uint32_t, SomeIPEvent> events{};
  std::unordered_map<uint16_t, SomeIPMethod> methods{};
  std::shared_ptr<data::Transfer> AddTransfer(const uint16_t transfer) {
    auto res = std::make_shared<data::Transfer>(transfer);
    this->transfers.push_back(res);
    return res;
  }

  void MethodCalled(
      const std::shared_ptr<simba::com::core::someip::SomeIpHeader> header,
      std::vector<std::uint8_t> data);
  void EventCalled(
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

  simba::core::ErrorCode AddEventCallback(const uint16_t service_id, const uint16_t event_id,
                                    SomeIPEvent callback) override;

  simba::core::ErrorCode SendEvent(
      const uint16_t event_id, const std::vector<uint8_t> payload) override;
  simba::core::ErrorCode Init() override;
  simba::core::ErrorCode LoadServiceList(const std::string& path) override;
  SomeIpController(const uint16_t service_id,
                   std::unique_ptr<soc::ISocket> socket,
                   std::shared_ptr<simba::core::logger::ILogger> logger_,
                   const soc::SocketConfig& config);
};
}  // namespace someip
}  // namespace com
}  // namespace simba

#endif  // COMUNICATION_CORE_SOMEIP_CONTROLLER_SOMEIP_CONTROLLER_H_
