/**
 * @file Igpio_controller.h
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief Interface for gpio controller
 * @version 0.1
 * @date 2024-06-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MW_GPIO_SERVER_CONTROLLER_IGPIO_CONTROLLER_H_
#define MW_GPIO_SERVER_CONTROLLER_IGPIO_CONTROLLER_H_
#include <optional>
#include "core/common/error_code.h"

namespace srp {
namespace gpio {

/**
 * @brief Class to controll GPIO
 * 
 * 
 * Class use GPIO_MW to controll gpio pins, allowed is only use as digital pins as IN/OUT
 * Controller dont allow to use digital pins as PWM, use PCA9685 to generate PWM
 * 
 * // TODO add functionality wchich allow to register for callback at pin state change
 * 
 */
class IGPIOController {
 public:
    /**
     * @brief Set the Pin Value
     * 
     * @
     * 
     * @param actuatorID 
     * @param value 
     * @return core::ErrorCode 
     */
    virtual core::ErrorCode SetPinValue(uint8_t actuatorID, int8_t value) = 0;
    /**
     * @brief Get the Pin Value
     * 
     * @param actuatorID 
     * @return std::optional<int8_t> 
     */
    virtual std::optional<int8_t> GetPinValue(uint8_t actuatorID) = 0;
    virtual ~IGPIOController() = default;
};
}  // namespace gpio
}  // namespace srp
#endif  // MW_GPIO_SERVER_CONTROLLER_IGPIO_CONTROLLER_H_
