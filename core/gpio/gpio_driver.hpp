/**
 * @file gpio_driver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_GPIO_GPIO_DRIVER_HPP_
#define CORE_GPIO_GPIO_DRIVER_HPP_

#include <stdint.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include "ara/log/logging_menager.h"
#include "Igpio_driver.hpp"
#include "core/file/file.hpp"

namespace srp {
namespace core {
namespace gpio {

class GpioDriver: public IGpioDriver{
 protected:
  std::unique_ptr<IFileHandler> file_;
  // ara::log::Logger gpio_logger_;

 public:
  explicit GpioDriver(std::unique_ptr<IFileHandler> file);
  ~GpioDriver();

  core::ErrorCode initializePin(const uint16_t& pinNumber, const direction_t& direction) override;
  /**
   * @brief Set the pin Value 
   * 
   * @param pinNumber 
   * @param value 
   * @return core::ErrorCode 
   */
  core::ErrorCode setValue(const uint16_t& pinNumber , const uint8_t& value) override;
  /**
   * @brief Set the pin Direction
   * 
   * @param pinNumber 
   * @param direction 
   * @return core::ErrorCode 
   */
  core::ErrorCode setDirection(const uint16_t& pinNumber,
               const direction_t& direction) override;

  /**
   * @brief Get Pin Value
   * 
   * @param pinNumber 
   * @return uint8_t 
   */
  uint8_t getValue(const uint16_t& pinNumber) override;
  /**
   * @brief Get the pin Direction
   * 
   * @param pinNumber 
   * @return direction_t 
   */
  direction_t getDirection(const uint16_t& pinNumber) override;
  /**
   * @brief Get the Active Pin Low
   * 
   * @param pinNumber 
   * @return true 
   * @return false 
   */
  static std::string getEndpointPath(const uint16_t& pinNumber, const std::string& endpoint);
  core::ErrorCode  unregisterPin(const uint16_t& pinNumber);
};
}  // namespace gpio
}  // namespace core
}  // namespace srp

#endif  // CORE_GPIO_GPIO_DRIVER_HPP_
