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

#include "core/gpio/IGPIO_digital_driver.h"


class MockGPIO : public simba::core::gpio::IgpioDigitalDriver {
 public:
  MOCK_METHOD((simba::core::ErrorCode), initializePin, (uint8_t, simba::core::gpio::direction_t), (override));
  MOCK_METHOD((uint8_t), getValue, (uint8_t), (override));
  MOCK_METHOD((simba::core::gpio::direction_t), getDirection, (uint8_t), (override));
  MOCK_METHOD((simba::core::ErrorCode), setValue, (uint8_t, uint8_t), (override));
  MOCK_METHOD((simba::core::ErrorCode), setDirection, (uint8_t, simba::core::gpio::direction_t), (override));
  virtual ~MockGPIO() = default;
};

#endif  // CORE_GPIO_MOCK_MOCK_GPIO_DRIVER_H_
