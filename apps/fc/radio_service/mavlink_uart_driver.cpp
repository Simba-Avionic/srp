/**
 * @file mavlink_uart_driver.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <poll.h>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
#include <optional>
#include "apps/fc/radio_service/mavlink_uart_driver.hpp"
namespace srp {
namespace core {
namespace uart {

bool MavUartDriver::WaitForData(int timeout_ms) {
    struct pollfd fds[1];
    fds[0].fd = serial_port;
    fds[0].events = POLLIN;

    int ret = poll(fds, 1, timeout_ms);
    if (ret > 0) {
        if (fds[0].revents & POLLIN) {
            return true;
        }
    }
    return false;
}


core::ErrorCode MavUartDriver::Write(const std::vector<uint8_t>& data) {
    auto n = write(serial_port, data.data(), data.size());
    if (n < 0) {
        return core::ErrorCode::kConnectionError;
    }
    if (n != data.size()) {
        return core::ErrorCode::kError;
    }
    return core::ErrorCode::kOk;
}

/**
 * @brief Open UART port
 * 
 * @param portName 
 * @param baudrate 
 * @param timeout  // value in deciseconds
 * @return true 
 * @return false 
 */
bool MavUartDriver::Open(const std::string& portName, const uint32_t& baudrate, const uint8_t timeout) {
    serial_port = open(portName.c_str(), O_RDWR | O_NOCTTY);
    if (serial_port == -1) {
        return false;
    }
    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0) {
        return false;
    }

    tty.c_cflag &= ~PARENB;  // Brak parzystości
    tty.c_cflag &= ~CSTOPB;  // Jeden bit stopu
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;  // 8 bitów danych
    tty.c_cflag &= ~CRTSCTS;  // Brak kontroli sprzętowej
    tty.c_lflag &= ~ICANON;   // Wyłączenie interpretacji nowych lini

    // // To Test 1
    tty.c_lflag &= ~ISIG;  // Disable interpretation of INTR, QUIT and SUSP
    // // To Test 2
    tty.c_lflag &= ~ECHO;  // Disable echo
    tty.c_lflag &= ~ECHOE;  // Disable erasure
    tty.c_lflag &= ~ECHONL;  // Disable new-line echo
    //
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);  // Turn off s/w flow ctrl
    //
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);  // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST;  // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR;  // Prevent conversion of newline to carriage return/line feed

    tty.c_cflag |= CREAD | CLOCAL;
    tty.c_cc[VTIME] = 10;  // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    cfsetospeed(&tty, baudrate);
    cfsetispeed(&tty, baudrate);

    return tcsetattr(serial_port, TCSANOW, &tty) == 0;
}



std::optional<std::vector<uint8_t>> MavUartDriver::Read(uint16_t size) {
    const size_t bytes_to_read = (size == 0) ? 512 : size;
    
    std::vector<uint8_t> res(bytes_to_read);
    
    ssize_t n = read(serial_port, res.data(), res.size());

    if (n > 0) {
        res.resize(n);
        return res;
    }

    return std::nullopt;
}
void MavUartDriver::Close() {
    close(serial_port);
}


}  // namespace uart
}  // namespace core
}  // namespace srp
