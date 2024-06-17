/**
 * @file mock_i2ccontroller.cc
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief Mock for an I2C Controller
 * @version 0.1
 * @date 2024-06-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef  MW_I2C_SERVICE_CONTROLLER_MOCK_MOCK_I2CCONTROLLER_H_
#define MW_I2C_SERVICE_CONTROLLER_MOCK_MOCK_I2CCONTROLLER_H_

#include <vector>
#include <optional>
#include <memory>
#include "gmock/gmock.h"
#include "mw/i2c_service/controller/i2c_controller.h"

namespace simba {
namespace mock {

class MockI2CController : public i2c::II2CController {
 public:
  MOCK_METHOD(std::optional<std::vector<uint8_t>>, SendData,
      (i2c::ACTION action, uint8_t address, const std::vector<uint8_t>& payload), (override));
  MOCK_METHOD(core::ErrorCode, Write, (const uint8_t address, const std::vector<uint8_t> data), (override));
  MOCK_METHOD(core::ErrorCode, PageWrite, (const uint8_t address, const std::vector<uint8_t> data), (override));
  MOCK_METHOD(std::optional<std::vector<uint8_t>>, Read,
        (const uint8_t address, const uint8_t reg, const uint8_t size), (override));
  MOCK_METHOD(std::optional<std::vector<uint8_t>>, WriteRead, (const uint8_t address,
        const uint8_t WriteData, const uint8_t ReadSize), (override));
  MOCK_METHOD(core::ErrorCode, Init, (std::unique_ptr<com::soc::StreamIpcSocket> socket), (override));
};
}  // namespace mock
}  // namespace simba
#endif  // MW_I2C_SERVICE_CONTROLLER_MOCK_MOCK_I2CCONTROLLER_H_
