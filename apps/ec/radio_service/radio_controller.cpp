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
#include "apps/ec/radio_service/radio_controller.hpp"

#include <chrono>  // NOLINT
#include "ara/log/log.h"
#include "core/common/condition.h"
namespace srp {
namespace apps {
namespace radio {
namespace {
    static constexpr auto KRadio_UART_path =     "/dev/ttyS1";
    static constexpr auto KRadio_UART_baudrate = core::uart::GetBaudrateConstant(57600);
    static constexpr auto kOpenRetryInterval = std::chrono::milliseconds(1000);
    static constexpr uint32_t kOpenWarnFirstAttempts = 10;
    static constexpr uint32_t kOpenWarnEveryNth = 30;
}

bool MavRadioController::Push(const std::vector<uint8_t>& data) {
    if (!uart_opened_.load()) {
        const uint32_t n = ++dropped_frames_;
        if (n <= kOpenWarnFirstAttempts || n % kOpenWarnEveryNth == 0) {
            mavl_logger.LogWarn() << "UART not open, dropping TX frame (dropped so far: " << n << ")";
        }
        return false;
    }
    return UartTxQueue.Push(data);
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
                    << msg.msgid << " len=" << msg.len;
      callback(msg);
    }
  }
}
void MavRadioController::OpenAndRxThreadLoop(const std::stop_token& t) {
  uint32_t attempt = 0;
  while (!t.stop_requested()) {
    ++attempt;
    if (uart_.Open(KRadio_UART_path, KRadio_UART_baudrate, 10)) {
      if (attempt > 1) {
        ara::log::LogInfo() << "UART " << KRadio_UART_path << " opened after " << attempt << " attempts";
      }
      uart_opened_.store(true);
      RxThreadLoop(t);
      return;
    }
    // Open() przy bledzie tcgetattr/tcsetattr nie zamyka fd - zwolnij go (dla -1 close() jest nieszkodliwe).
    uart_.Close();
    if (attempt <= kOpenWarnFirstAttempts || attempt % kOpenWarnEveryNth == 0) {
      ara::log::LogWarn() << "Failed to open UART: " << KRadio_UART_path << " (attempt " << attempt
                          << "), retrying in " << kOpenRetryInterval.count() << " ms";
    }
    core::condition::wait_for(kOpenRetryInterval, t);
  }
}

void MavRadioController::TxThreadLoop(const std::stop_token& t) {
    while (!t.stop_requested() && !uart_opened_.load()) {
      core::condition::wait_for(std::chrono::milliseconds(100), t);
    }
    while (!t.stop_requested()) {
      auto package_opt = UartTxQueue.Get(t);
      if (!package_opt.has_value()) {
        continue;
      }
      ara::log::LogWarn() << "Send: " << package_opt.value();
      uart_.Write(package_opt.value());
    }
}

void MavRadioController::Init(RadioMsgCallback callback, const std::stop_token& t) {
    this->callback = callback;
    uart_transmit_thread = std::jthread([this](const std::stop_token& t) { TxThreadLoop(t); });
    uart_receive_thread = std::jthread([this](const std::stop_token& t) { OpenAndRxThreadLoop(t); });
}

MavRadioController::MavRadioController(): mavl_logger{ara::log::LoggingMenager::GetInstance()->
          CreateLogger("MAVL", "", ara::log::LogLevel::kDebug)} {}

}  // namespace radio
}  // namespace apps
}  // namespace srp
