/**
 * @file modbus.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef CORE_PD_33X_MODBUS_MODBUS_HPP_
#define CORE_PD_33X_MODBUS_MODBUS_HPP_
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include "core/pd-33x/rs485/rs485.hpp"
namespace srp {
namespace core {

class MODBUS {
 protected:
  std::unique_ptr<core::IRS485> rs485_;
  uint8_t slave_id;
  uint16_t Calculate_CRC16(const std::vector<uint8_t>& data);
  bool Validate_CRC16(const std::vector<uint8_t>& data);
 public:
  bool Init(const RS485_conf_t& config, const uint8_t slave_id,
              std::unique_ptr<core::IRS485> rs485 = std::make_unique<core::RS485>());
  std::optional<std::vector<uint8_t>> SendRequest(uint8_t function_code, uint16_t start_addr, uint16_t quantity);
};

}  // namespace core
}  // namespace srp

#endif  // CORE_PD_33X_MODBUS_MODBUS_HPP_
