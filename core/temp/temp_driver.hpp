/**
 * @file TempDriver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_TEMP_TEMP_DRIVER_HPP_
#define CORE_TEMP_TEMP_DRIVER_HPP_

#include <optional>
#include <vector>
#include <cstdint>
#include <string>
#include "ara/core/result.h"
#include "core/temp/Itemp_driver.hpp"

namespace srp {
namespace core {
namespace temp {

class TempDriver : public ITempDriver {
 public:
  // TODO(GH): Add Check that sensor exist.
  // TODO(GH): Add print all sensors value on startup as ara::logInfo
  TempDriver();
  ara::core::Result<double> ReadTemp(const std::string& sensorPhysicalID) override;
  ara::core::Result<bool> SetResolution(const std::string& sensorPhysicalID, uint8_t resolution) override;
  ara::core::Result<uint8_t> ReadResolution(const std::string& sensorPhysicalID) override;
  ara::core::Result<std::vector<std::string>> GetAllAccessibleSensor() override;
  ara::core::Result<uint16_t> GetResponseTime(const std::string& sensorPhysicalID) override;
};

}  // namespace temp
}  // namespace core
}  // namespace srp

#endif  // CORE_TEMP_TEMP_DRIVER_HPP_
