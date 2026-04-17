/**
 * @file radio_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-04-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef APPS_FC_RADIO_SERVICE_RADIO_CONTROLLER_HPP_
#define APPS_FC_RADIO_SERVICE_RADIO_CONTROLLER_HPP_
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <thread>  // NOLINT
#include "core/common/error_code.h"
#include "core/common/wait_queue.h"
#include "ara/log/logging_menager.h"
#include "apps/fc/radio_service/mavlink_uart_driver.hpp"
#include "simba/mavlink.h"

namespace srp {
namespace apps {
namespace radio {

using RadioMsgCallback = std::function<void(const mavlink_message_t&)>;

class MavRadioController {
 private:
  const ara::log::Logger& mavl_logger;
  core::uart::MavUartDriver uart_;
  core::WaitQueue<std::vector<uint8_t>, 20> UartTxQueue;
  RadioMsgCallback callback;
  std::jthread uart_transmit_thread;
  std::jthread uart_receive_thread;
  void RxThreadLoop(const std::stop_token& t);
  void TxThreadLoop(const std::stop_token& t);
 public:
  void Push(const std::vector<uint8_t>& data);
  void TxLoop();
  void Init(RadioMsgCallback callback, const std::stop_token& t);
  MavRadioController();
};

}  // namespace radio
}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RADIO_SERVICE_RADIO_CONTROLLER_HPP_
