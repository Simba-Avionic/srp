/**
 * @file sim_uart_driver.cpp
 * @brief Sim UART (core/uart) - bez sprzetu, kolejka RX i dziennik TX sterowane przez core/sim/sim_ctl.hpp.
 *
 * Open dziala na dowolnej sciezce, Read/WaitForData zwracaja bajty wstrzykniete poleceniem ctl
 * "uart <port> rx ...", Write laduje w events.jsonl i state/uart/<port>.tx. Konfiguracja (UartConfig) jest
 * zapisywana w events.jsonl ("op":"config"); timeout odczytu = vtime z konfiguracji.
 * Polecenia ctl uart: patrz core/sim/README.md.
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

bool UartDriver::Open(const std::string& portName, const uint32_t& baudrate, const uint8_t /*timeout*/) {
    return Open(portName, UartConfig::Legacy(baudrate));
}

bool UartDriver::Open(const std::string& portName, const UartConfig& config) {
    if (config.baudrate == B0) {
        logger_.LogError() << "UartDriver::Open: unsupported baudrate (B0) for " << portName;
        return false;
    }
    Close();
    serial_port = sim::UartSim::Get().Open(portName, config.vtime);
    if (serial_port == -1) {
        return false;
    }
    config_ = config;
    sim::Event("uart", "\"op\":\"config\",\"port\":\"" + portName + "\",\"baud\":" +
                           std::to_string(static_cast<unsigned>(config.baudrate)) +
                           ",\"data_bits\":" + std::to_string(config.data_bits) +
                           ",\"stop_bits\":" + std::to_string(config.stop_bits) +
                           ",\"raw\":" + (config.raw ? "true" : "false") +
                           ",\"vmin\":" + std::to_string(config.vmin) +
                           ",\"vtime\":" + std::to_string(config.vtime));
    return true;
}

bool UartDriver::WaitForData(int timeout_ms) {
    return sim::UartSim::Get().WaitForData(serial_port, timeout_ms);
}

std::optional<std::vector<uint8_t>> UartDriver::Read(const uint16_t size) {
    return sim::UartSim::Get().Read(serial_port, (size == 0) ? config_.default_read_size : size);
}

void UartDriver::Close() {
    if (serial_port < 0) {
        return;
    }
    sim::UartSim::Get().Close(serial_port);
    serial_port = -1;
}

UartDriver::~UartDriver() {
    Close();
}

}  // namespace uart
}  // namespace core
}  // namespace srp
