/**
 * @file test_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "gtest/gtest.h"

#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "communication-core/sockets/mock/mockSocket.h"
#include "core/common/error_code.h"

using ::testing::_;
using ::testing::Return;

TEST(GPIO_CONTROLLER, TESTS) {
  MockSocket sock_;
  ON_CALL(sock_, Transmit(_, _, _))
            .WillByDefault(Return(simba::core::ErrorCode::kOk));
  simba::gpio::GPIOController gpio_(&sock_);
  EXPECT_EQ(gpio_.SetPinValue(12, simba::gpio::Value::HIGH), simba::core::ErrorCode::kOk);
  EXPECT_EQ(gpio_.SetPinValue(14, simba::gpio::Value::ERROR), simba::core::ErrorCode::kError);
  EXPECT_EQ(gpio_.GetPinValue(21), simba::gpio::Value::HIGH);
  EXPECT_EQ(gpio_.Init(12), simba::core::ErrorCode::kOk);
}
