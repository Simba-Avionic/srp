/**
 * @file mock_gpio_controller.h
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief Mock gpio controller
 * @version 0.1
 * @date 2024-06-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MW_GPIO_SERVER_CONTROLLER_MOCK_MOCK_GPIO_CONTROLLER_H_
#define MW_GPIO_SERVER_CONTROLLER_MOCK_MOCK_GPIO_CONTROLLER_H_

#include "gmock/gmock.h"
#include "mw/gpio_server/controller/Igpio_controller.h"
namespace simba {
namespace mock {
class MOCKGPIOController : public gpio::IGPIOController {
 public:
  MOCK_METHOD(core::ErrorCode, SetPinValue, (uint8_t actuatorID, int8_t value), (override));
  MOCK_METHOD(std::optional<int8_t>, GetPinValue, (uint8_t actuatorID), (override));
};
}  // namespace mock
}  // namespace simba
#endif  // MW_GPIO_SERVER_CONTROLLER_MOCK_MOCK_GPIO_CONTROLLER_H_
