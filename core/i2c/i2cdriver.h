/**
 * @file i2cdriver.h
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief This file defines I2C driver
 * @version 0.1
 * @date 2024-01-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_I2C_I2CDRIVER_H_
#define CORE_I2C_I2CDRIVER_H_
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <unistd.h>
#include <vector>
#include "core/common/error_code.h"
#include "core/results/result.h"
#include <string>
namespace simba {
namespace core {
class I2C{
 public:
    ErrorCode init(const std::string& path);
    ErrorCode read(const uint8_t address, const uint8_t reg, std::vector<uint8_t>& vect);
    ErrorCode Write(const uint8_t address, const uint8_t reg, std::vector<uint8_t> data);
 private:
    int file = -1;
    int32_t i2c_smbus_access(char read_write,
    uint8_t command, int size, union i2c_smbus_data *data);
    Result<uint8_t> i2c_smbus_read_byte_data(uint8_t command);
    ErrorCode i2c_smbus_write_byte_data(uint8_t command, uint8_t value);
};
}  // namespace core
}  // namespace simba
#endif  // CORE_I2C_I2CDRIVER_H_
