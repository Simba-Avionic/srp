/**
 * @file Iads7828.h
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief Interface for ads7828
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MW_I2C_SERVICE_CONTROLLER_ADS7828_IADS7828_H_
#define MW_I2C_SERVICE_CONTROLLER_ADS7828_IADS7828_H_
#include <vector>
#include <optional>
#include <cstdint>

namespace simba {
namespace i2c {
class IADS7828 {
 protected:
  virtual std::optional<uint8_t> GetConfigData(const uint8_t& channel) const = 0;
  virtual std::optional<uint16_t> GetAdcRawRead(const uint8_t& channel) const = 0;
 public:
  virtual std::optional<float> GetAdcVoltage(const uint8_t& channel) const = 0;
  virtual ~IADS7828() = default;
};
}  // namespace i2c
}  // namespace simba
#endif  // MW_I2C_SERVICE_CONTROLLER_ADS7828_IADS7828_H_
