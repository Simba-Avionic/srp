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
namespace simba {
namespace core {
namespace uart {

bool UartDriver::Open(const std::string& portName, const speed_t& baudrate) {
    serial_port = open(portName.c_str(), O_RDONLY | O_NOCTTY);
    if (serial_port == -1) {
        return false;
    }
    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0) {
        return false;
    }
    cfsetospeed(&tty, baudrate);
    cfsetispeed(&tty, baudrate);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;
    tcsetattr(serial_port, TCSANOW, &tty);
    return true;
}
std::optional<std::vector<char>> UartDriver::Read(const uint16_t size) {
    char read_buf[256];
    int n = read(serial_port, read_buf, ((size == 0) ? sizeof(read_buf) : size));
    if (n > 0) {
        std::vector<char> res(n);
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
}  // namespace simba
