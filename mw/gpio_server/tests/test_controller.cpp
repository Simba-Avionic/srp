/**
 * @file test_controller.cpp
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "gtest/gtest.h"

#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "communication-core/sockets/mock/mockSocket.h"
#include "core/common/error_code.h"

class SetPinTest : public ::testing::TestWithParam<
                        std::tuple<uint16_t, simba::gpio::Value, simba::core::ErrorCode>> {
};

INSTANTIATE_TEST_SUITE_P(SetPinTestParameters, SetPinTest,
    ::testing::Values(
        std::make_tuple(0, simba::gpio::Value::ERROR, simba::core::ErrorCode::kError),
        std::make_tuple(0, simba::gpio::Value::LOW, simba::core::ErrorCode::kOk),
        std::make_tuple(0, simba::gpio::Value::HIGH, simba::core::ErrorCode::kOk),
        std::make_tuple(0, simba::gpio::Value::ERROR, simba::core::ErrorCode::kError),
        std::make_tuple(12, simba::gpio::Value::ERROR, simba::core::ErrorCode::kError),
        std::make_tuple(14, simba::gpio::Value::HIGH, simba::core::ErrorCode::kOk),
        std::make_tuple(21, simba::gpio::Value::LOW, simba::core::ErrorCode::kOk),
        std::make_tuple(255, simba::gpio::Value::ERROR, simba::core::ErrorCode::kError),
        std::make_tuple(255, simba::gpio::Value::LOW, simba::core::ErrorCode::kOk),
        std::make_tuple(255, simba::gpio::Value::HIGH, simba::core::ErrorCode::kOk)
    )
);


TEST_P(SetPinTest, CONTROLLER_SET_PIN_VALUE_CHECK) {
  MockSocket sock_;
  auto params = GetParam();
  uint16_t actuatorID = std::get<0>(params);
  simba::gpio::Value value = std::get<1>(params);
  simba::core::ErrorCode ExpectedErrorCode = std::get<2>(params);

  if (value != simba::gpio::Value::ERROR) {
      EXPECT_CALL(sock_, Transmit(::testing::_, ::testing::_, ::testing::_))
          .WillOnce(::testing::Return(simba::core::ErrorCode::kError))
          .WillOnce(::testing::Return(ExpectedErrorCode));
  }
  simba::gpio::GPIOController gpio_(&sock_);
  EXPECT_EQ(gpio_.SetPinValue(actuatorID, value), simba::core::ErrorCode::kError);
  EXPECT_EQ(gpio_.SetPinValue(actuatorID, value), ExpectedErrorCode);
}


TEST(GPIO_CONTROLLER, TEST_NULLPTR) {
    simba::gpio::GPIOController gpio_(nullptr);
    EXPECT_EQ(gpio_.SetPinValue(0, simba::gpio::Value::HIGH),
    simba::core::ErrorCode::kInitializeError);
}

class GetPinTest : public ::testing::TestWithParam<
                         std::tuple<uint16_t, simba::gpio::Value>> {
 protected:
  void SetUp() override {
    GTEST_SKIP();
  }
};

INSTANTIATE_TEST_SUITE_P(GetPinTestParameters, GetPinTest,
    ::testing::Values(
        std::make_tuple(0, simba::gpio::Value::HIGH),
        std::make_tuple(0, simba::gpio::Value::LOW),
        std::make_tuple(12, simba::gpio::Value::ERROR),
        std::make_tuple(14, simba::gpio::Value::HIGH),
        std::make_tuple(21, simba::gpio::Value::LOW),
        std::make_tuple(255, simba::gpio::Value::HIGH),
        std::make_tuple(255, simba::gpio::Value::LOW)
    )
);

TEST_P(GetPinTest, CONTROLLER_GET_PIN_VALUE_CHECK) {
  MockSocket sock_;
  ON_CALL(sock_, Transmit(::testing::_, ::testing::_ , ::testing::_))
            .WillByDefault(::testing::Return(simba::core::ErrorCode::kOk));
  simba::gpio::GPIOController gpio_(&sock_);
  auto params = GetParam();
  uint16_t actuatorID = std::get<0>(params);
  simba::gpio::Value value = std::get<1>(params);

  gpio_.SetPinValue(actuatorID, value);
  EXPECT_EQ(gpio_.GetPinValue(actuatorID), value);
}
