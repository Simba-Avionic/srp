/**
 * @file GPIO_digital_driver.h
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef CORE_GPIO_GPIO_DIGITAL_DRIVER_H_
#define CORE_GPIO_GPIO_DIGITAL_DRIVER_H_

#include "IGPIO_digital_driver.h"

#include <stdint.h>
#include <unordered_map>
#include <string>
#include <vector>

namespace simba {
namespace core {
namespace gpio {

class GpioDigitalDriver: public IgpioDigitalDriver{
 public:
  GpioDigitalDriver();
  ~GpioDigitalDriver();

  core::ErrorCode initializePin(uint8_t pinNumber, direction_t direction) override;
  /**
   * @brief Set the pin Value 
   * 
   * @param pinNumber 
   * @param value 
   * @return core::ErrorCode 
   */
  core::ErrorCode setValue(uint8_t pinNumber , uint8_t value) override;
  /**
   * @brief Set the pin Direction
   * 
   * @param pinNumber 
   * @param direction 
   * @return core::ErrorCode 
   */
  core::ErrorCode setDirection(uint8_t pinNumber,
               direction_t direction) override;

  /**
   * @brief Get Pin Value
   * 
   * @param pinNumber 
   * @return uint8_t 
   */
  uint8_t getValue(uint8_t pinNumber) override;
  /**
   * @brief Get the pin Direction
   * 
   * @param pinNumber 
   * @return direction_t 
   */
  direction_t getDirection(uint8_t pinNumber) override;
  /**
   * @brief Get the Active Pin Low
   * 
   * @param pinNumber 
   * @return true 
   * @return false 
   */
 private:
  std::string getEndpointPath(uint8_t pinNumber, std::string endpoint);
  core::ErrorCode  unregisterPin(uint8_t pinNumber);
  const std::string path = "/sys/class/gpio";
};
}  // namespace gpio
}  // namespace core
}  // namespace simba

#endif  // CORE_GPIO_GPIO_DIGITAL_DRIVER_H_
