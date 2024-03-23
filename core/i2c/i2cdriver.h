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
#include <string>
#include <optional>
#include "core/common/error_code.h"
namespace simba {
namespace core {
class I2C{
 public:
    ErrorCode init(const std::string& path);
    ErrorCode init();
    ~I2C() {
      close(this->file);
    }
    std::optional<std::vector<uint8_t>> Read(const uint8_t address, const int buffor_size);
    std::optional<std::vector<uint8_t>> Read(const uint8_t address, const uint8_t reg, const int buffor_size);
    ErrorCode Write(const uint8_t address, const uint8_t reg, std::vector<uint8_t> data);
 private:
    int file = -1;
};
}  // namespace core
}  // namespace simba
#endif  // CORE_I2C_I2CDRIVER_H_
