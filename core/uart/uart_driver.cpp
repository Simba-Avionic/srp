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
#include <csignal>
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
    if (static_cast<std::size_t>(n) != data.size()) {
        return core::ErrorCode::kError;
    }
    return core::ErrorCode::kOk;
}

bool UartDriver::Open(const std::string& portName, const uint32_t& baudrate, UartConfig config) {
    serial_port = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (serial_port == -1) {
        return false;
    }
    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0) {
        return false;
    }
    cfsetospeed(&tty, baudrate);
    cfsetispeed(&tty, baudrate);
    if (!config.enable_partity) {
        tty.c_cflag &= ~PARENB;  // Brak parzystości
    } else {
        tty.c_cflag |= PARENB;  // ENABLE PARTITY
    }
    if (config.two_stop_bits) {
        tty.c_cflag |= CSTOPB;  // TWO STOP BITS
    } else {
        tty.c_cflag &= ~CSTOPB;  // Jeden bit stopu
    }
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= config.bits;
    tty.c_cflag &= ~CRTSCTS;  // Brak kontroli sprzętowej
    tty.c_cflag |= CREAD | CLOCAL;
    tty.c_cc[VTIME] = config.read_wait_time;
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
void UartDriver::SetRxCallback(UartCallbackHandler rx_callback) {
    this->rx_callback = rx_callback;
}
bool UartDriver::StartRxThread(const std::string& app_name) {
    if (!this->rx_callback) {
        return false;
    }
    if (!rx_thread) {
        return false;
    }
    this->rx_thread = std::make_unique<std::jthread>(
        [&](std::stop_token stoken) {
            while (!stoken.stop_requested()) {
                auto val = this->Read();
                if (val.has_value()) {
                    this->rx_callback(val.value());
                }
            }
    });
    std::string name = "Uart_Thread" + app_name;
    pthread_setname_np(this->rx_thread->native_handle(), name.c_str());
    return true;
}
void UartDriver::Close() {
    if (rx_thread) {
        rx_thread->request_stop();
        rx_thread->join();
    }
    close(serial_port);
}

}  // namespace uart
}  // namespace core
}  // namespace srp
