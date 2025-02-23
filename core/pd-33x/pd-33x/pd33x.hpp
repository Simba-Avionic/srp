/**
 * @file pd33x.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef CORE_PD_33X_PD_33X_PD33X_HPP_
#define CORE_PD_33X_PD_33X_PD33X_HPP_
#include <optional>
#include <utility>
#include <memory>
#include <vector>
#include "core/pd-33x/modbus/modbus.hpp"
namespace srp {
namespace core {
class PD33X {
 private:
  std::unique_ptr<MODBUS> modbus_;
 public:
  bool Init(const RS485_conf_t& config, const uint8_t slave_id = kslave_id,
                std::unique_ptr<MODBUS> modbus = std::make_unique<MODBUS>());
  std::optional<float> ReadP1();
  std::optional<float> ReadT1();
  float parseFloat(const std::vector<uint8_t>& data);
};

}  // namespace core
}  // namespace srp

#endif  // CORE_PD_33X_PD_33X_PD33X_HPP_
