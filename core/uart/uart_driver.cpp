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

#include "core/uart/uart_driver.hpp"
#include "ara/core/result.h"
#include "ara/log/log.h"
#include "ara/com/com_error_domain.h"
namespace simba {
namespace core {
namespace uart {

bool UartDriver::Open(const std::string& portName, const int32_t& mode) {
    file = open(portName.c_str(), mode);
    if (file == -1) {
        return false;
    }
    fcntl(file, F_SETFL, 0);
    ara::log::LogInfo() << "Succesful Open uart";
    return true;
}
void UartDriver::Configure(const uint32_t& baudrate){
    struct termios options;
    tcgetattr(file, &options);
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);

    options.c_cflag = baudrate | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(file, TCIFLUSH);
    tcsetattr(file, TCSANOW, &options);
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

ara::core::Result<std::vector<uint8_t>> UartDriver::Read(const uint8_t& size) {
    std::vector<uint8_t> buffer(size);
    memset(buffer.data(), 0, buffer.size());
    int bytes_read = read(file, buffer.data(), buffer.size() - 1);
    if (bytes_read < size) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kCommunicationStackError, "Invalid read");
    }
    ara::log::LogInfo() << "succesfull read darta:" << buffer;
    return buffer;
}
void UartDriver::Close() {
    close(file);
}

}  // namespace uart
}  // namespace core
}  // namespace simba