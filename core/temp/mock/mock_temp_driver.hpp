/**
 * @file mock_temp_driver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_TEMP_MOCK_MOCK_TEMP_DRIVER_HPP_
#define CORE_TEMP_MOCK_MOCK_TEMP_DRIVER_HPP_
#include <gmock/gmock.h>
#include <vector>
#include <string>
#include "core/temp/Itemp_driver.hpp"

namespace simba {
namespace mock {

class MockTempDriver : public simba::core::temp::ITempDriver {
 public:
  MOCK_METHOD(ara::core::Result<double>, ReadTemp, (const std::string& sensorPhysicalID), (override));
  MOCK_METHOD(ara::core::Result<bool>, SetResolution,
                                (const std::string& sensorPhysicalID, uint8_t resolution), (override));
  MOCK_METHOD(ara::core::Result<uint8_t>, ReadResolution, (const std::string& sensorPhysicalID), (override));
  MOCK_METHOD(ara::core::Result<std::vector<std::string>>, GetAllAccessibleSensor, (), (override));
  MOCK_METHOD(ara::core::Result<uint16_t>, GetResponseTime, (const std::string& sensorPhysicalID), (override));
};

}  // namespace mock
}  // namespace simba

#endif  // CORE_TEMP_MOCK_MOCK_TEMP_DRIVER_HPP_
