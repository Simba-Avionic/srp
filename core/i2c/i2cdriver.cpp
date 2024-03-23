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
    this->file = open(path.c_str(), O_RDWR);
    return this->file < 0 ? ErrorCode::kError : ErrorCode::kOk;
}
ErrorCode I2C::init() {
    this->file = open("/dev/i2c-2", O_RDWR);
    if (this->file < 0) {
        return ErrorCode::kError;
    }
    return ErrorCode::kOk;
}
std::optional<std::vector<uint8_t>> I2C::Read(const uint8_t address, const int buffor_size) {
    if (ioctl(file, I2C_SLAVE, address) < 0) {
        return {};
    }
    char buffor[kMaxBufSize];
    if (read(this->file, buffor, buffor_size) != buffor_size) {
        return {};
    }
    std::vector<uint8_t> vec(buffor, buffor + sizeof(buffor));
    return std::optional<std::vector<uint8_t>>{std::vector<uint8_t>{vec}};
}
std::optional<std::vector<uint8_t>> I2C::Read(const uint8_t address, const uint8_t reg, const int buffor_size) {
    if (ioctl(file, I2C_SLAVE, address) < 0) {
        return {};
    }
    char buffer[kMaxBufSize];
    buffer[0] = reg;
    if (read(file, buffer+1, buffor_size) != buffor_size) {
        return {};
    }
    std::vector<uint8_t> vec(buffer + 1, buffer + 1 + buffor_size);
    return vec;
}

ErrorCode I2C::Write(const uint8_t address, const uint8_t reg, std::vector<uint8_t> data) {
    if (ioctl(this->file, I2C_SLAVE, address) < 0) {
        return ErrorCode::kInitializeError;
    }
    std::vector<uint8_t> buffer(data.size() + 1);
        buffer[0] = reg;
        std::copy(data.begin(), data.end(), buffer.begin() + 1);
    if (write(file, data.data(), data.size()) != data.size()) {
        return ErrorCode::kError;
    }
    return ErrorCode::kOk;
}
}  // namespace core
}  // namespace simba
