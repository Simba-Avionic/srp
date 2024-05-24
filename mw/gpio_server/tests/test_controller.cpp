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
                        std::tuple<uint8_t, int8_t>> {
};

INSTANTIATE_TEST_SUITE_P(SetPinTestParameters, SetPinTest,
    ::testing::Values(
        std::make_tuple(0, 0),
        std::make_tuple(0, 1),
        std::make_tuple(0, 0),
        std::make_tuple(0, 1),
        std::make_tuple(12, 0),
        std::make_tuple(14, 1),
        std::make_tuple(21, 0),
        std::make_tuple(255, 1),
        std::make_tuple(255, 0),
        std::make_tuple(255, 1)
    )
);


TEST_P(SetPinTest, CONTROLLER_SET_PIN_VALUE_CHECK) {
  MockStreamSocket sock_;
  auto params = GetParam();
  uint16_t actuatorID = std::get<0>(params);
  int8_t value = std::get<1>(params);
  EXPECT_CALL(sock_, Transmit(::testing::_, ::testing::_, ::testing::_))
      .WillOnce(::testing::Return(std::vector<uint8_t>{1}))
      .WillOnce(::testing::Return(std::vector<uint8_t>{0}));

  simba::gpio::GPIOController gpio_(&sock_);
  EXPECT_EQ(gpio_.SetPinValue(actuatorID, value), simba::core::ErrorCode::kOk);
  EXPECT_EQ(gpio_.SetPinValue(actuatorID, value), simba::core::ErrorCode::kError);
}


TEST(GPIO_CONTROLLER, TEST_NULLPTR) {
    simba::gpio::GPIOController gpio_(nullptr);
    EXPECT_EQ(gpio_.SetPinValue(0, 1),
    simba::core::ErrorCode::kInitializeError);
}

class GetPinTest : public ::testing::TestWithParam<
                         std::tuple<uint8_t, int8_t>> {
 protected:
  void SetUp() override {
    GTEST_SKIP();
  }
};

INSTANTIATE_TEST_SUITE_P(GetPinTestParameters, GetPinTest,
    ::testing::Values(
        std::make_tuple(0, 1),
        std::make_tuple(0, 0),
        std::make_tuple(14, 1),
        std::make_tuple(21, 0),
        std::make_tuple(255, 1),
        std::make_tuple(255, 0)
    )
);

TEST_P(GetPinTest, CONTROLLER_GET_PIN_VALUE_CHECK) {
  MockStreamSocket sock_;
  ON_CALL(sock_, Transmit(::testing::_, ::testing::_ , ::testing::_))
            .WillByDefault(::testing::Return(std::vector<uint8_t>{1}));
  simba::gpio::GPIOController gpio_(&sock_);
  auto params = GetParam();
  uint16_t actuatorID = std::get<0>(params);
  int8_t value = std::get<1>(params);

  gpio_.SetPinValue(actuatorID, value);
  EXPECT_EQ(gpio_.GetPinValue(actuatorID), value);
}
