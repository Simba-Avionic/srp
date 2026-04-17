/**
 * @file radio_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-04-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "apps/fc/radio_service/radio_controller.hpp"
#include "ara/log/log.h"
namespace srp {
namespace apps {
namespace radio {
namespace {
    static constexpr auto KRadio_UART_path =     "/dev/ttyS1";
    static constexpr auto KRadio_UART_baudrate = core::uart::GetBaudrateConstant(57600);
}

void MavRadioController::Push(const std::vector<uint8_t>& data) {
    UartTxQueue.Push(data);
}

void MavRadioController::RxThreadLoop(const std::stop_token& t) {
  mavlink_message_t msg;
  mavlink_status_t status;
  while (!t.stop_requested()) {
    if (!uart_.WaitForData(250)) {
      continue;
    }
    auto bytes_opt = uart_.Read(0);
    if (!bytes_opt.has_value()) {
      continue;
    }
    for (uint8_t i = 0; i < bytes_opt.value().size(); i++) {
      if (!mavlink_parse_char(MAVLINK_COMM_0, bytes_opt.value()[i], &msg, &status)) {
        continue;
      }
      mavl_logger.LogDebug() << "Parsed MAVLink msg id="
                          + std::to_string(msg.msgid)
                          + " len=" + std::to_string(msg.len);
      callback(msg);
    }
  }
}
void MavRadioController::TxThreadLoop(const std::stop_token& t) {
    while (!t.stop_requested()) {
      auto package_opt = UartTxQueue.Get(t);
      if (!package_opt.has_value()) {
        continue;
      }
      uart_.Write(package_opt.value());
    }
}

void MavRadioController::Init(RadioMsgCallback callback, const std::stop_token& t) {
    if (!this->uart_.Open(KRadio_UART_path, KRadio_UART_baudrate, 10)) {
        ara::log::LogError() << "Failed to open UART: " <<  KRadio_UART_path;
        exit(1);
    }
    this->callback = callback;
    uart_transmit_thread = std::jthread([this](const std::stop_token& t) { TxThreadLoop(t); });
    uart_receive_thread = std::jthread([this](const std::stop_token& t) { RxThreadLoop(t); });
}

MavRadioController::MavRadioController(): mavl_logger{ara::log::LoggingMenager::GetInstance()->
          CreateLogger("MAVL", "", ara::log::LogLevel::kDebug)} {}

}  // namespace radio
}  // namespace apps
}  // namespace srp
