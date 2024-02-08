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
namespace simba {
namespace core {
int32_t I2C::i2c_smbus_access(char read_write, uint8_t command, int size, union i2c_smbus_data *data) {
    i2c_smbus_ioctl_data args;
    args.read_write = read_write;
    args.command = command;
    args.size = size;
    args.data = data;
    return ioctl(this->file, I2C_SMBUS, &args);
}
Result<uint8_t> I2C::i2c_smbus_read_byte_data(const uint8_t command) {
    union i2c_smbus_data data;
    if (i2c_smbus_access(I2C_SMBUS_READ, command, I2C_SMBUS_BYTE_DATA, &data) != 0) {
        return Result<uint8_t>{};
    }
    return Result<uint8_t>(0xFF & data.byte);
}
ErrorCode I2C::init(const std::string&  path) {
    this->file = open(path.c_str(), O_RDWR);
    if (this->file < 0) {
        return ErrorCode::kError;
    }
    return ErrorCode::kOk;
}
Result<std::vector<uint8_t>> I2C::Read(const uint8_t address, const uint8_t reg) {
    std::vector<uint8_t> result;
    if (ioctl(this->file, I2C_SLAVE, address) < 0) {
        return Result<std::vector<uint8_t>>{};
    }
    Result<uint8_t> data = i2c_smbus_read_byte_data(reg);
    if (!data.HasValue()) {
        return Result<std::vector<uint8_t>>{};
    }
    while (data.Value() >= 0) {
        result.push_back(data.Value());
        data = i2c_smbus_read_byte_data(reg);
    }
    return Result<std::vector<uint8_t>>{result};
}
ErrorCode I2C::Write(const uint8_t address, const uint8_t reg, std::vector<uint8_t> data) {
    if (ioctl(this->file, I2C_SLAVE, address) < 0) {
        return ErrorCode::kInitializeError;
    }
    if (write(file, data.data(), data.size()) != data.size()) {
        return ErrorCode::kError;
    }
    return ErrorCode::kOk;
}
}  // namespace core
}  // namespace simba
