/**
 * @file eepromController.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_I2C_SERVICE_EEPROM_CONTROLLER_EEPROMCONTROLLER_HPP_
#define MW_I2C_SERVICE_EEPROM_CONTROLLER_EEPROMCONTROLLER_HPP_

#include <optional>
#include <vector>

#include "communication-core/sockets/ipc_socket.h"
#include "mw/i2c_service/eeprom/factory/eeprom_factory.hpp"

namespace simba {
namespace core {

class EepromController {
 public:
  std::optional<uint8_t> ReadData(uint8_t address, uint8_t reg);
  std::vector<uint8_t> ReadData(uint8_t address);
  void Init(uint16_t app_id);
  ErrorCode WriteData(uint8_t address, uint8_t reg, uint8_t data);
  ErrorCode WriteData(uint8_t address, std::vector<uint8_t> data);
 private:
  uint8_t transferID{0};
  uint16_t appID{0};
  com::soc::IpcSocket sock_;
  static i2c::EepromMsgFactory factory_;
};
}  // namespace core
}  // namespace simba




#endif  // MW_I2C_SERVICE_EEPROM_CONTROLLER_EEPROMCONTROLLER_HPP_
