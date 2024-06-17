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

namespace simba {
namespace gpio {
class IGPIOController {
 public:
    virtual core::ErrorCode SetPinValue(uint8_t actuatorID, int8_t value) = 0;
    virtual std::optional<int8_t> GetPinValue(uint8_t actuatorID) = 0;
    virtual ~IGPIOController() = default;
};
}  // namespace gpio
}  // namespace simba
#endif  // MW_GPIO_SERVER_CONTROLLER_IGPIO_CONTROLLER_H_
