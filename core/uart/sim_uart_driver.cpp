/**
 * @file sim_uart_driver.cpp
 * @brief Sim UART (core/uart) - bez sprzetu, kolejka RX i dziennik TX sterowane przez core/sim/sim_ctl.hpp.
 *
 * Open dziala na dowolnej sciezce, Read zwraca bajty wstrzykniete poleceniem ctl "uart <port> rx ...",
 * Write laduje w events.jsonl i state/uart/<port>.tx. Polecenia ctl uart: patrz core/sim/README.md.
 *
 * @copyright Copyright (c) 2025
 */
#include <optional>
#include <string>
#include <vector>

#include "core/sim/sim_uart.hpp"
#include "core/uart/uart_driver.hpp"

namespace srp {
namespace core {
namespace uart {

UartDriver::UartDriver():
    serial_port(-1),
    logger_(ara::log::LoggingMenager::GetInstance()->CreateLogger(
        "uart", "", ara::log::LogLevel::kWarn)) {}

core::ErrorCode UartDriver::Write(const std::vector<uint8_t>& data) {
    return sim::UartSim::Get().Write(serial_port, data) ? core::ErrorCode::kOk
                                                         : core::ErrorCode::kConnectionError;
}

bool UartDriver::Open(const std::string& portName, const uint32_t& /*baudrate*/, const uint8_t timeout) {
    serial_port = sim::UartSim::Get().Open(portName, timeout);
    return serial_port != -1;
}

std::optional<std::vector<uint8_t>> UartDriver::Read(const uint16_t size) {
    // jak prawdziwy driver: size == 0 oznacza odczyt do 256 B
    return sim::UartSim::Get().Read(serial_port, (size == 0) ? 256 : size);
}

void UartDriver::Close() {
    sim::UartSim::Get().Close(serial_port);
}

}  // namespace uart
}  // namespace core
}  // namespace srp
