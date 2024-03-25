/**
 * @file i2cdriver.cpp
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief This file defines I2C driver
 * @version 0.1
 * @date 2024-01-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "i2cdriver.h"

#include <algorithm>

namespace {
    constexpr uint8_t kMaxBufSize = 128;
}

namespace simba {
namespace core {

ErrorCode I2C::init(const std::string&  path) {
    this->path = path;
    return ErrorCode::kOk;
}
ErrorCode I2C::init() {
    this->path = "/dev/i2c-2";
    return ErrorCode::kOk;
}

void I2C::openFile() {
    this->file = open(this->path.c_str(), O_RDWR);
}
void I2C::closeFile() {
    close(this->file);
}

core::ErrorCode I2C::connectTo(uint8_t deviceAddress) {
    this->address = deviceAddress;
    if (ioctl(file, I2C_SLAVE, address) < 0) {
       return core::ErrorCode::kInitializeError;
    }
    return core::ErrorCode::kOk;
}
std::optional<uint8_t> I2C::ReadAndWrite( const uint8_t reg) {
    if (write(file, &reg, 1) != 1) {
        return {};
    }
    uint8_t data;
    if (read(file, &data, 1) != 1) {
        return {};
    }
    return std::optional<uint8_t>{data};
}

ErrorCode I2C::Write(const uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;

    if (write(file, &buf, 2) != 1) {
        return ErrorCode::kError;
    }
    return ErrorCode::kOk;
}

/**
 * @brief Dont work??
 * 
 * @param address 
 * @return std::optional<uint8_t> 
 */
std::optional<uint8_t> I2C::Read(const uint8_t address) {
    uint8_t buffor = 0x10;
    if (read(file, &buffor, sizeof(buffor)) != sizeof(buffor)) {
        return {};
    }
    return std::optional<uint16_t>{buffor};
}


}  // namespace core
}  // namespace simba
