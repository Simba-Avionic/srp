/**
 * @file i2cdriver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-04-03
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef CORE_I2C_I2CDRIVER_HPP_
#define CORE_I2C_I2CDRIVER_HPP_

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "core/common/error_code.h"
#include "core/i2c/Ii2cdriver.hpp"
#include "ara/log/logging_menager.h"
namespace simba {
namespace core {
namespace i2c {

class I2CDriver : public II2CDriver {
 private:
  int i2cFile;
  const ara::log::Logger& i2c_logger_;
 public:
  I2CDriver();
  core::ErrorCode Init() override;
  core::ErrorCode Ioctl(const uint8_t address, const uint16_t type = I2C_SLAVE) override;
  /**
   * @brief Function to write data to device on i2c bus
   *
   * @param data pair ( req, data)
   * @return core::ErrorCode
   */
  core::ErrorCode Write(const std::vector<uint8_t>& RegData) override;
  core::ErrorCode Write(const uint8_t& data) override;
  std::optional<std::vector<uint8_t>> Read(const uint8_t size = 1) override;
  core::ErrorCode PageWrite(std::vector<uint8_t> data) override;
  std::optional<std::vector<uint8_t>> ReadWrite(const uint8_t& reg, const uint8_t size = 1) override;
};
}  // namespace i2c
}  // namespace core
}  // namespace simba

#endif  // CORE_I2C_I2CDRIVER_HPP_
