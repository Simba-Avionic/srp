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

#include "core/common/error_code.h"


namespace simba {
namespace core {
namespace gpio {

enum direction_t{
    IN,
    OUT,
    ERROR,
};

class IgpioDigitalDriver{
 public:
    IgpioDigitalDriver() {}


    virtual core::ErrorCode initializePin(uint8_t pinNumber, direction_t direction) = 0;

    /**
     * @brief Set the output Pin Value
     * 
     * @param pinNumber 
     * @param value 
     */
    virtual core::ErrorCode setValue(uint8_t pinNumber , uint8_t value) = 0;

    /**
     * @brief Set the Pin direction (Pin direction must be set to output)
     * 
     * @param pinNumber 
     * @param direction 
     */
    virtual core::ErrorCode setDirection(uint8_t pinNumber,
                                direction_t direction) = 0;

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
};
}  // namespace gpio
}  // namespace core
}  // namespace simba

#endif  // CORE_GPIO_IGPIO_DIGITAL_DRIVER_H_
