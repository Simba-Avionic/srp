/**
 * @file IGPIO_driver.h
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief Interface for digital GPIO drivers
 * @version 0.1
 * @date 2023-10-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CORE_GPIO_IGPIO_DIGITAL_DRIVER_H_
#define CORE_GPIO_IGPIO_DIGITAL_DRIVER_H_

#include <stdint.h>

namespace simba {
namespace core {

enum direction_t{
    IN,
    OUT,
};
enum gpio_error_t{
    FILE_NOT_FOUND,
    PIN_NOT_SET_TO_OUTPUT,
    OK
};

class IgpioDigitalDriver{
 public:
    IgpioDigitalDriver() {}

    /**
     * @brief Set the output Pin Value
     * 
     * @param pinNumber 
     * @param value 
     */
    virtual gpio_error_t setValue(uint8_t pinNumber , uint8_t value) = 0;

    /**
     * @brief Set the Pin direction (Pin direction must be set to output)
     * 
     * @param pinNumber 
     * @param direction 
     */
    virtual gpio_error_t setDirection(uint8_t pinNumber, direction_t direction) = 0;

    /**
     * @brief Set the base value Pin to activate
     * 
     * @param pinNumber 
     * @param value 
     */
    virtual gpio_error_t setActivePinLow(uint8_t pinNumber, bool value) = 0;

    /**
     * @brief read Pin Value
     * 
     * @param pinNumber 
     * @return uint8_t 
     */
    virtual uint8_t getValue(uint8_t pinNumber) = 0;
    /**
     * @brief read Pin direction value
     * 
     * @param pinNumber 
     * @return direction_t 
     */
    virtual direction_t getDirection(uint8_t pinNumber) = 0;
    virtual bool getActivePinLow(uint8_t pinNumber) = 0;
};
}  // namespace core
}  // namespace simba

#endif  // CORE_GPIO_IGPIO_DIGITAL_DRIVER_H_
