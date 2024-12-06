/**
 * @file Igpio_driver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_GPIO_IGPIO_DRIVER_HPP_
#define CORE_GPIO_IGPIO_DRIVER_HPP_

#include <stdint.h>
#include <string>

#include "core/common/error_code.h"
#include "core/file/file.hpp"


namespace simba {
namespace core {
namespace gpio {

enum direction_t{
    IN,
    OUT,
    ERROR,
};

class IGpioDriver{
 public:
    virtual core::ErrorCode initializePin(const uint16_t& pinNumber, const direction_t& direction) = 0;

    /**
     * @brief Set the output Pin Value
     * 
     * @param pinNumber 
     * @param value 
     */
    virtual core::ErrorCode setValue(const uint16_t &pinNumber , const uint8_t& value) = 0;

    /**
     * @brief Set the Pin direction (Pin direction must be set to output)
     * 
     * @param pinNumber 
     * @param direction 
     */
    virtual core::ErrorCode setDirection(const uint16_t &pinNumber,
                                const direction_t& direction) = 0;

    /**
     * @brief read Pin Value
     * 
     * @param pinNumber 
     * @return uint8_t 
     */
    virtual uint8_t getValue(const uint16_t &pinNumber) = 0;
    /**
     * @brief read Pin direction value
     * 
     * @param pinNumber 
     * @return direction_t 
     */
    virtual direction_t getDirection(const uint16_t &pinNumber) = 0;
    static std::string getEndpointPath(const uint16_t& pinNumber, const std::string& endpoint);
    virtual core::ErrorCode  unregisterPin(const uint16_t& pinNumber) = 0;
};
}  // namespace gpio
}  // namespace core
}  // namespace simba

#endif  // CORE_GPIO_IGPIO_DRIVER_HPP_
