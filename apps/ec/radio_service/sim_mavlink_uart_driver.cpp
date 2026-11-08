/**
 * @file sim_mavlink_uart_driver.cpp
 * @brief Sim MavUartDriver (EC radio_service) - ten sam model portu co core/uart/sim_uart_driver.cpp.
 *
 * Open dziala na dowolnej sciezce (np. /dev/ttyS1), Read/WaitForData zwracaja bajty wstrzykniete
 * poleceniem ctl "uart <port> rx ...", Write laduje w events.jsonl i state/uart/<port>.tx.
 * Polecenia ctl uart: patrz core/sim/README.md.
 *
 * @copyright Copyright (c) 2025
 */
#include <optional>
#include <string>
#include <vector>

#include "apps/ec/radio_service/mavlink_uart_driver.hpp"
#include "core/sim/sim_uart.hpp"

namespace srp {
namespace core {
namespace uart {

bool MavUartDriver::WaitForData(int timeout_ms) {
    return sim::UartSim::Get().WaitForData(serial_port, timeout_ms);
}

core::ErrorCode MavUartDriver::Write(const std::vector<uint8_t>& data) {
    return sim::UartSim::Get().Write(serial_port, data) ? core::ErrorCode::kOk
                                                         : core::ErrorCode::kConnectionError;
}

bool MavUartDriver::Open(const std::string& portName, const uint32_t& /*baudrate*/, const uint8_t timeout) {
    serial_port = sim::UartSim::Get().Open(portName, timeout);
    return serial_port != -1;
}

std::optional<std::vector<uint8_t>> MavUartDriver::Read(uint16_t size) {
    // jak prawdziwy driver: size == 0 oznacza odczyt do 512 B
    return sim::UartSim::Get().Read(serial_port, (size == 0) ? 512 : size);
}

void MavUartDriver::Close() {
    sim::UartSim::Get().Close(serial_port);
}

}  // namespace uart
}  // namespace core
}  // namespace srp
