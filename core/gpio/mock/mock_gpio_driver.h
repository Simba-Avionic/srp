/**
 * @file mock_gpio_driver.h
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_GPIO_MOCK_MOCK_GPIO_DRIVER_H_
#define CORE_GPIO_MOCK_MOCK_GPIO_DRIVER_H_

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "core/gpio/Igpio_driver.hpp"

class MockGPIO : public srp::core::gpio::IGpioDriver {
 public:
  MOCK_METHOD((srp::core::ErrorCode), initializePin,
                      (const uint16_t&, const srp::core::gpio::direction_t&), (override));
  MOCK_METHOD((uint8_t), getValue, (const uint16_t&), (override));
  MOCK_METHOD((srp::core::gpio::direction_t), getDirection, (const uint16_t&), (override));
  MOCK_METHOD((srp::core::ErrorCode), setValue, (const uint16_t&, const uint8_t&), (override));
  MOCK_METHOD((srp::core::ErrorCode), setDirection,
                          (const uint16_t&, const srp::core::gpio::direction_t&), (override));
  virtual ~MockGPIO() = default;
};

#endif  // CORE_GPIO_MOCK_MOCK_GPIO_DRIVER_H_
