/**
 * @file Ii2cdriver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_I2C_II2CDRIVER_HPP_
#define CORE_I2C_II2CDRIVER_HPP_

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
class II2C {
 public:
    virtual ErrorCode init(const std::string& path) = 0;
    virtual std::optional<std::vector<uint8_t>> Read(const uint8_t address, const uint8_t reg);
    virtual ErrorCode Write(const uint8_t address, const uint8_t reg, std::vector<uint8_t> data);
};
}  // namespace core
}  // namespace simba

#endif  // CORE_I2C_II2CDRIVER_HPP_
