/**
 * @file mock_ads7828.h
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief Mock of ads7828
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MW_I2C_SERVICE_CONTROLLER_MOCK_MOCK_ADS7828_H_ 
#define MW_I2C_SERVICE_CONTROLLER_MOCK_MOCK_ADS7828_H_ 
#include "gmock/gmock.h"
#include "mw/i2c_service/controller/ads7828/Iads7828.h"

namespace simba {
namespace mock {
class MockADS7828 : public i2c::IADS7828 {
 public:
  MOCK_METHOD(std::optional<uint8_t>, GetConfigData, (const uint8_t& channel));
  MOCK_METHOD(std::optional<uint16_t>, GetAdcRawRead, (const uint8_t& channel));
  MOCK_METHOD(std::optional<float>, GetAdcVoltage, (const uint8_t& channel));
};
}  // namespace mock
}  // namespace simba
#endif  // MW_I2C_SERVICE_CONTROLLER_MOCK_MOCK_ADS7828_H_
 