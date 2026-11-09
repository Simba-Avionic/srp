/**
 * @file uart_driver.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-19
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
#include <optional>
#include "core/uart/uart_driver.hpp"
namespace srp {
namespace core {
namespace uart {

UartDriver::UartDriver():
    serial_port(-1),
    logger_(ara::log::LoggingMenager::GetInstance()->CreateLogger(
        "uart", "", ara::log::LogLevel::kWarn)) {}

core::ErrorCode UartDriver::Write(const std::vector<uint8_t>& data) {
    auto n = write(serial_port, data.data(), data.size());
    if (n < 0) {
        return core::ErrorCode::kConnectionError;
    }
    if (static_cast<size_t>(n) != data.size()) {
        return core::ErrorCode::kError;
    }
    return core::ErrorCode::kOk;
}

/**
 * @brief Open UART port with the legacy (canonical) configuration
 *
 * @param portName
 * @param baudrate termios constant (B9600, B115200, ...)
 * @param timeout  ignored as before (VTIME is fixed by UartConfig::Legacy())
 * @return true
 * @return false
 */
bool UartDriver::Open(const std::string& portName, const uint32_t& baudrate, const uint8_t /*timeout*/) {
    return Open(portName, UartConfig::Legacy(baudrate));
}

/**
 * @brief Open UART port with an explicit configuration (see uart_config.hpp for the presets)
 */
bool UartDriver::Open(const std::string& portName, const UartConfig& config) {
    if (config.baudrate == B0) {  // B0 = hang up; also what GetBaudrateConstant() returns for an unsupported speed
        logger_.LogError() << "UartDriver::Open: unsupported baudrate (B0) for " << portName;
        return false;
    }
    Close();
    serial_port = open(portName.c_str(), O_RDWR | O_NOCTTY);
    if (serial_port == -1) {
        return false;
    }
    config_ = config;
    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0) {
        Close();
        return false;
    }
    config.ApplyTo(&tty);
    const int rc = tcsetattr(serial_port, TCSANOW, &tty);
    if (config.fail_on_apply_error && rc != 0) {
        Close();
        return false;
    }
    return true;
}

bool UartDriver::WaitForData(int timeout_ms) {
    struct pollfd fds[1];
    fds[0].fd = serial_port;
    fds[0].events = POLLIN;
    const int ret = poll(fds, 1, timeout_ms);
    return ret > 0 && (fds[0].revents & POLLIN);
}

std::optional<std::vector<uint8_t>> UartDriver::Read(const uint16_t size) {
    const size_t bytes_to_read = (size == 0) ? config_.default_read_size : size;
    std::vector<uint8_t> res(bytes_to_read);
    const ssize_t n = read(serial_port, res.data(), res.size());
    if (n > 0) {
        res.resize(n);
        return res;
    }
    return std::nullopt;
}
void UartDriver::Close() {
    if (serial_port < 0) {
        return;  // not open or already closed
    }
    close(serial_port);
    serial_port = -1;
}

UartDriver::~UartDriver() {
    Close();
}

}  // namespace uart
}  // namespace core
}  // namespace srp
