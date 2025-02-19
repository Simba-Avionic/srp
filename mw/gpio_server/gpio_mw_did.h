/**
 * @file example_did.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MW_GPIO_SERVER_GPIO_MW_DID_H_
#define MW_GPIO_SERVER_GPIO_MW_DID_H_
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <bitset>
#include "ara/log/log.h"
#include "core/gpio/Igpio_driver.hpp"
#include "ara/diag/generic_data_identifier.h"
#include "ara/diag/uds_error_domain.h"
namespace srp {
namespace mw {

struct GpioConf{
  uint16_t pinNum;
  core::gpio::direction_t direction;
};

class GpioMWDID : public ara::diag::GenericDiD {
 private:
  std::shared_ptr<core::gpio::IGpioDriver> gpio_driver_;
  const std::unordered_map<uint8_t, GpioConf> gpio_config;
  /**
   * @brief Callback for 0x22 UDS job (Read data by id) *optional
   *
   * @return DiagResponse
   */
  ara::core::Result<ara::diag::OperationOutput> Read() noexcept override {
    std::bitset<32> res;
    std::vector<uint8_t> vec_res(4);
    for (const auto &pair : gpio_config) {
      auto val = this->gpio_driver_->getValue(pair.second.pinNum);
      res[pair.first] = val;
    }
    for (size_t i = 0; i < 4; ++i) {
      vec_res[i] = static_cast<uint8_t>((res >> (i * 8)).to_ulong());
    }
    return ara::diag::OperationOutput{vec_res};
  }
  ara::core::Result<void> Write(
    const std::vector<uint8_t> &payload) noexcept override {
  ara::log::LogInfo() << "Receive diag write req with payload size: "
      << (uint16_t)payload.size() << ",p[0] = " << payload[0] << ",p[1] = " << payload[1];
  if (payload.size() != 2) {
    ara::log::LogWarn() << "Invalid payload size ";
    return ara::diag::MakeErrorCode(
      ara::diag::UdsDiagErrc::kInvalidMessageLengthFormat);
  }
  if (payload[0] > 60 || (payload[1] != 0 && payload[1] != 1)) {
    ara::log::LogWarn() << "Invalid data in payload";
    return ara::diag::MakeErrorCode(
      ara::diag::UdsDiagErrc::kInvalidKey);
  }
  auto it = this->gpio_config.find(payload[0]);
  if (it == this->gpio_config.end()) {
    ara::log::LogWarn() << "Cant find pin with id: " << payload[0];
    return ara::diag::MakeErrorCode(
      ara::diag::UdsDiagErrc::kInvalidKey);
  }
  this->gpio_driver_->setValue(it->second.pinNum, payload[1]);
  ara::log::LogInfo() << "OK";
  return {};
}

 public:
  GpioMWDID(const ara::core::InstanceSpecifier &specifier, std::shared_ptr<
        core::gpio::IGpioDriver> gpio_driver, const std::unordered_map<uint8_t, GpioConf>& config)
                        : ara::diag::GenericDiD{specifier}, gpio_driver_(gpio_driver), gpio_config(config) {
      }
};

}  // namespace mw
}  // namespace srp

#endif  // MW_GPIO_SERVER_GPIO_MW_DID_H_
