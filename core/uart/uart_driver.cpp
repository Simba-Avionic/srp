/**
 * @file uart_driver.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h> 

#include <optional>

#include "core/uart/uart_driver.hpp"
#include "ara/core/result.h"
#include "ara/log/log.h"
#include "ara/com/com_error_domain.h"
namespace simba {
namespace core {
namespace uart {

bool UartDriver::Open(const std::string& portName) {
    file = open(portName.c_str(), O_RDWR);
    if (file < 0) {
        return false;
    }
    ara::log::LogInfo() << "Succesful Open uart";
    return true;
}
void UartDriver::Configure(const uart_config_t& config){
    this->config_ = config;
    struct termios tty;
    tcgetattr(file, &tty);
    cfsetispeed(&tty, config_.baudrate);
    cfsetospeed(&tty, config_.baudrate);
    if (config_.enablePartity) {
        tty.c_cflag |= PARENB;
    } else {
        tty.c_cflag &= ~PARENB;
    }
    if (config_.setTwoStopBits) {
        tty.c_cflag |= CSTOPB;
    } else {
        tty.c_cflag &= ~CSTOPB;
    }
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= config_.baudrate;
    if (config_.enableHardwareControl) {
        tty.c_cflag |= CRTSCTS;
    } else {
        tty.c_cflag &= ~CRTSCTS;
    }
    tty.c_cc[VTIME] = 1;
    tty.c_cc[VMIN] = 0;
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
    ara::log::LogInfo() << "Setup configuration succesfully";
}
bool UartDriver::Write(const std::vector<uint8_t>& data) {
    auto count = write(file, data.data(), data.size());
    if (count < data.size()) {
        return false;
    }
    return true;
}

bool UartDriver::ReadAvailable() {
    int bytes_available = 0;
    if (ioctl(file, FIONREAD, &bytes_available) < 0) {
        return false;
    }
    return bytes_available > 0;
}

std::optional<std::vector<uint8_t>> UartDriver::Read() {
    char read_buf[256];
    int bytes_read = read(file, &read_buf, sizeof(read_buf));
    if (bytes_read  <= 0) {
        ara::log::LogError() <<"Invalid read";
        return std::nullopt;
    }
    std::vector<uint8_t> result(bytes_read);
    std::memcpy(result.data(), read_buf, bytes_read);
    ara::log::LogInfo() << "succesfull read darta:" << result;
    return result;
}
void UartDriver::Close() {
    close(file);
}

}  // namespace uart
}  // namespace core
}  // namespace simba