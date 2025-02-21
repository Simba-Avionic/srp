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

core::ErrorCode UartDriver::Write(const std::vector<uint8_t>& data) {
    auto n = write(serial_port, data.data(), data.size());
    if (n < 0) {
        return core::ErrorCode::kConnectionError;
    }
    if (n != data.size()) {
        return core::ErrorCode::kError;
    }
    return core::ErrorCode::kOk;
}

bool UartDriver::Open(const std::string& portName, const speed_t& baudrate) {
    serial_port = open(portName.c_str(), O_RDWR | O_NOCTTY);
    if (serial_port == -1) {
        return false;
    }
    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0) {
        return false;
    }
    cfsetospeed(&tty, baudrate);
    cfsetispeed(&tty, baudrate);
    tty.c_cflag &= ~PARENB; // Brak parzystości
    tty.c_cflag &= ~CSTOPB; // Jeden bit stopu
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8; // 8 bitów danych
    tty.c_cflag &= ~CRTSCTS; // Brak kontroli sprzętowej
    tty.c_cflag |= CREAD | CLOCAL;
    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;
    tcsetattr(serial_port, TCSANOW, &tty);
    return true;
}
std::optional<std::vector<uint8_t>> UartDriver::Read(const uint16_t size) {
    uint8_t read_buf[256];
    int n = read(serial_port, read_buf, ((size == 0) ? sizeof(read_buf) : size));
    if (n > 0) {
        std::vector<uint8_t> res(n);
        std::memcpy(res.data(), read_buf, n);
        return res;
    }
    return std::nullopt;
}
void UartDriver::Close() {
    close(serial_port);
}

}  // namespace uart
}  // namespace core
}  // namespace srp
