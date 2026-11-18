/**
 * Copyright (c) 2024
 *
 * @file serial_port_uart.cpp
 */
#include "core/radio/e220/serial_port_uart.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace {
constexpr std::uint32_t kWriteTimeoutMs = 1000;

/// Speed in bit/s for a termios constant; 0 = not an E220 UART speed.
std::uint32_t BaudBps(speed_t speed) {
  for (const std::uint32_t bps : {1200U, 2400U, 4800U, 9600U, 19200U, 38400U, 57600U, 115200U}) {
    if (uart::GetBaudrateConstant(bps) == speed) {
      return bps;
    }
  }
  return 0;
}
}  // namespace

bool SerialPortUart::Open(const std::string& portName, const uint32_t& baudrate, const uint8_t) {
  return Open(portName, uart::UartConfig::Legacy(baudrate));
}

bool SerialPortUart::Open(const std::string& portName, const uart::UartConfig& config) {
  const std::uint32_t bps = BaudBps(config.baudrate);
  if (bps == 0 || config.parity != uart::Parity::kNone) {
    return false;
  }
  port_.Close();
  return port_.Open(portName, bps).ok;
}

bool SerialPortUart::WaitForData(std::chrono::milliseconds timeout, const std::stop_token& stoken) {
  const auto r = port_.Poll(false, static_cast<std::uint32_t>(timeout.count()), stoken);
  // A device error also wakes the caller: Read() then reports it.
  return r.readable || r.status == serial::IoStatus::kError;
}

std::optional<std::vector<uint8_t>> SerialPortUart::Read(const uint16_t size) {
  std::vector<uint8_t> buf(size == 0 ? 256 : size);
  const auto r = port_.TryRead(buf.data(), buf.size());
  if (r.status == serial::IoStatus::kOk) {
    buf.resize(r.bytes);
    return buf;
  }
  if (r.status == serial::IoStatus::kTimeout) {
    // Nothing to read. A non-blocking TryRead() does not notice a vanished device (HUP), Poll() does.
    if (port_.Poll(false, 0, {}).status == serial::IoStatus::kError) {
      return std::nullopt;
    }
    return std::vector<uint8_t>{};
  }
  return std::nullopt;  // closed or device lost
}

core::ErrorCode SerialPortUart::Write(const std::vector<uint8_t>& data) {
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(kWriteTimeoutMs);  // NOLINT
  std::size_t sent = 0;
  while (sent < data.size()) {
    const auto r = port_.TryWrite(data.data() + sent, data.size() - sent);
    if (r.status == serial::IoStatus::kOk) {
      sent += r.bytes;
      continue;
    }
    if (r.status != serial::IoStatus::kTimeout) {
      return core::ErrorCode::kConnectionError;
    }
    // Output buffer full: wait for room until the write deadline.
    const auto now = std::chrono::steady_clock::now();  // NOLINT
    const auto left = std::chrono::duration_cast<std::chrono::milliseconds>(deadline - now);
    if (left.count() <= 0) {
      return core::ErrorCode::kError;
    }
    const auto p = port_.Poll(true, static_cast<std::uint32_t>(left.count()), {});
    if (p.status == serial::IoStatus::kClosed || p.status == serial::IoStatus::kError) {
      return core::ErrorCode::kConnectionError;
    }
  }
  return core::ErrorCode::kOk;
}

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
