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

#include <stdint.h>
#include <string>

#include "IGPIO_digital_driver.h"

namespace simba {
namespace core {

class GpioDigitalDriver:IgpioDigitalDriver{
 public:
  GpioDigitalDriver();
  explicit GpioDigitalDriver(std::string path);

  gpio_error_t setValue(uint8_t pinNumber , uint8_t value) override;
  gpio_error_t setDirection(uint8_t pinNumber, direction_t direction) override;
  gpio_error_t setActivePinLow(uint8_t pinNumber, bool value) override;
  uint8_t getValue(uint8_t pinNumber) override;
  direction_t getDirection(uint8_t pinNumber) override;
  bool getActivePinLow(uint8_t pinNumber) override;
 private:
  std::string getEndpointPath(uint8_t pinNumber, std::string endpoint);

  std::string path;
};

}  // namespace core
}  // namespace simba

#endif  // CORE_GPIO_GPIO_DIGITAL_DRIVER_H_
