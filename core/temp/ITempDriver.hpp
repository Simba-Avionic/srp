/**
 * @file ITempDriver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_TEMP_ITEMPDRIVER_HPP_
#define CORE_TEMP_ITEMPDRIVER_HPP_

#include <optional>
#include <vector>
#include <cstdint>
#include <string>
#include "ara/core/result.h"

namespace simba {
namespace core {
namespace temp {

class ITempDriver {
 public:
  virtual ~ITempDriver() = default;
  virtual ara::core::Result<double> ReadTemp(const std::string& sensorPhysicalID) = 0;
  virtual ara::core::Result<bool> SetResolution(const std::string& sensorPhysicalID, uint8_t resolution) = 0;
  virtual ara::core::Result<uint8_t> ReadResolution(const std::string& sensorPhysicalID) = 0;
  virtual ara::core::Result<std::vector<std::string>> GetAllAccessibleSensor() = 0;
  virtual ara::core::Result<uint16_t> GetResponseTime(const std::string& sensorPhysicalID) = 0;
};

}  // namespace temp
}  // namespace core
}  // namespace simba

#endif  // CORE_TEMP_ITEMPDRIVER_HPP_
