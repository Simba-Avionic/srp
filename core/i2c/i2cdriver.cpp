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

std::optional<uint8_t> I2C::ReadAndWrite(const uint8_t address, const uint8_t reg) {
    auto file = open(this->path.c_str(), O_RDWR);
    if (ioctl(file, I2C_SLAVE, address) < 0) {
        close(file);
       return {};
    }
    if (write(file, &reg, 1) != 1) {
        close(file);
        return {};
    }
    uint8_t data;
    if (read(file, &data, 1) != 1) {
        close(file);
        return {};
    }
    close(file);
    return std::optional<uint8_t>{data};
}

ErrorCode I2C::Write(const uint8_t address, const uint8_t reg, uint8_t data) {
    auto file = open(path.c_str(), O_RDWR);
    if (ioctl(file, I2C_SLAVE, address) < 0) {
        close(file);
        return ErrorCode::kInitializeError;
    }
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;

    if (write(file, &buf, 2) != 1) {
        close(file);
        return ErrorCode::kError;
    }
    close(file);
    return ErrorCode::kOk;
}

/**
 * @brief Dont work??
 * 
 * @param address 
 * @return std::optional<uint8_t> 
 */
std::optional<uint8_t> I2C::Read(const uint8_t address) {
    auto file = open(this->path.c_str(), O_RDWR);
    if (ioctl(file, I2C_SLAVE, address) < 0) {
        close(file);
        return {};
    }
    uint8_t buffor = 0x10;
    if (read(file, &buffor, sizeof(buffor)) != sizeof(buffor)) {
        close(file);
        return {};
    }
    close(file);
    return std::optional<uint16_t>{buffor};
}


}  // namespace core
}  // namespace simba
