/**
 * @file test_pca9685.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <iostream>

#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "mw/gpio_server/controller/mock/mock_gpio_controller.h"
#include "mw/i2c_service/controller/mock/mock_i2ccontroller.h"
#include "tools/cpp/runfiles/runfiles.h"
using bazel::tools::cpp::runfiles::Runfiles;

namespace {
  constexpr uint8_t PCA9685_ADDRESS = 0x70;
  static const std::string FILE_PREFIX = "mw/i2c_service/tests/pca_files/";
}
class TestWrapper : public srp::i2c::PCA9685 {
 public:
  std::vector<uint8_t> GetData(const uint8_t &channel, const uint16_t &pos) {
    return this->GenerateData(channel, pos);
  }
  std::optional<std::unordered_map<uint8_t, srp::i2c::Servo>> read_config(std::string file_path) {
    return this->ReadConfig(file_path);
  }
  void SetDB(std::unordered_map<uint8_t, srp::i2c::Servo> db) {
    this->db_ = db;
  }
  std::optional<uint8_t> read_pos(const uint8_t &actuator_id) {
    return this->ReadServoPosition(actuator_id);
  }
  srp::core::ErrorCode TestSetGpio(std::unique_ptr<srp::gpio::IGPIOController> gpio) {
    return this->setGPIO(std::move(gpio));
  }
  srp::core::ErrorCode TestSetI2C(std::unique_ptr<srp::i2c::II2CController> i2c) {
    return this->setI2C(std::move(i2c));
  }
  void TestMosfetFunc(const uint8_t &mosfet_id, const uint8_t &mosfet_time) {
    this->MosfetFunc(mosfet_id, mosfet_time);
  }
  srp::core::ErrorCode TestSetServo(uint8_t channel, uint16_t pos) {
    return this->SetServo(channel, pos);
  }
};


TEST(TestBasicRead, TestRead) {
  TestWrapper wrapper;
  EXPECT_TRUE(wrapper.read_config(FILE_PREFIX+"basic.json"));
}
TEST(TestBasicRead, NoFile) {
  TestWrapper wrapper;
  EXPECT_FALSE(wrapper.read_config(FILE_PREFIX+"asdadsbasic.json").has_value());
}
TEST(TestBasicRead, NoDataInFile) {
  TestWrapper wrapper;
  EXPECT_FALSE(wrapper.read_config(FILE_PREFIX+"empty.json").has_value());
}
TEST(TestBasicRead, MissingField) {
  TestWrapper wrapper;
  auto conf = wrapper.read_config(FILE_PREFIX+"invalid_conf.json");
  ASSERT_TRUE(conf.has_value());
  EXPECT_EQ(conf.value().size(), 0);
}

TEST(TESTCheckServoPOS, TestPOS) {
  TestWrapper wrapper{};
  auto db_ = wrapper.read_config(FILE_PREFIX+"basic.json");
  wrapper.SetDB(db_.value());
  EXPECT_FALSE(wrapper.read_pos(0).has_value());
  EXPECT_TRUE(wrapper.read_pos(1).has_value());
  EXPECT_EQ(wrapper.read_pos(1).value(), 0);
}

class DataCreateTest : public ::testing::TestWithParam<std::tuple<uint8_t, uint16_t, std::vector<uint8_t>>>{
};

// tuple<channel, pos, expected>
INSTANTIATE_TEST_SUITE_P(DataCreateTestParams, DataCreateTest, ::testing::Values(
  std::make_tuple(0, 0, std::vector<uint8_t>{0, 1, 0xFE, 121, 0x6, 0, 0x7, 0, 0x8, 0, 0x9, 0}),
  std::make_tuple(2, 2048, std::vector<uint8_t>{0, 1, 0xFE, 121, 14, 0, 15, 0, 16, 0, 17, 8})
));

TEST_P(DataCreateTest, GenerateDataTest) {
    TestWrapper wrapper{};
    auto params = GetParam();
    uint8_t channel = std::get<0>(params);
    uint16_t pos = std::get<1>(params);
    auto expected = std::get<2>(params);
    EXPECT_EQ(wrapper.GetData(channel, pos), expected);
}

class PCA9685ConfigTests : public ::testing::TestWithParam<std::tuple<uint16_t, std::string, uint16_t,
            uint16_t, uint8_t, uint8_t, bool, uint8_t, bool, uint16_t, uint16_t, uint8_t, uint8_t>> {
};

INSTANTIATE_TEST_CASE_P(
  PCA9685Test123,
  PCA9685ConfigTests,
  ::testing::Values(
    std::make_tuple(1, "c_t_1.json", 1000, 2000, 0, 0, true, 1, true, 1500, 2500, 10, 20),
    std::make_tuple(2, "c_t_2.json", 1500, 2000, 1, 0, false, 0, false, 0, 0, 0, 0),
    std::make_tuple(3, "c_t_3.json", 1500, 2000, 3, 0, true, 2, false, 0, 0, 50, 250),
    std::make_tuple(4, "c_t_4.json", 1500, 2000, 1, 0, false, 0, true, 1000, 2500, 0, 0)

));

TEST_P(PCA9685ConfigTests, READ_DATA_TEST) {
  TestWrapper pca9685_;
  std::string path = std::get<1>(GetParam());
  std::optional<std::unordered_map<uint8_t, srp::i2c::Servo>> result = pca9685_.read_config(FILE_PREFIX + path);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  auto servo_map = result.value();
  EXPECT_TRUE(servo_map.find(std::get<0>(GetParam())) != servo_map.end());
  const srp::i2c::Servo& servo = servo_map.at(std::get<0>(GetParam()));
  EXPECT_EQ(servo.on_pos, std::get<2>(GetParam()));
  EXPECT_EQ(servo.off_pos, std::get<3>(GetParam()));
  EXPECT_EQ(servo.channel, std::get<4>(GetParam()));
  EXPECT_EQ(servo.position, std::get<5>(GetParam()));
  EXPECT_EQ(servo.need_mosfet, std::get<6>(GetParam()));
  EXPECT_EQ(servo.mosfet_id, std::get<7>(GetParam()));
  EXPECT_EQ(servo.need_loosening, std::get<8>(GetParam()));
  EXPECT_EQ(servo.on_loosening, std::get<9>(GetParam()));
  EXPECT_EQ(servo.off_loosening, std::get<10>(GetParam()));
  EXPECT_EQ(servo.servo_delay, std::get<11>(GetParam()));
  EXPECT_EQ(servo.mosfet_time, std::get<12>(GetParam()));
};

TEST(TestPCAController, MosfetFuncTest) {
    TestWrapper wrapper{};
    auto mock_gpio = std::make_unique<srp::mock::MOCKGPIOController>();
    const uint8_t mosfet_id = 1;
    const uint8_t mosfet_time = 1;
    EXPECT_CALL(*mock_gpio, SetPinValue(mosfet_id, ::testing::_))
      .Times(2);
    wrapper.TestSetGpio(std::move(mock_gpio));
    wrapper.TestMosfetFunc(mosfet_id, mosfet_time);
}

TEST(TestPCAController, SetServoTest) {
    TestWrapper wrapper{};
    auto mock_i2c = std::make_unique<srp::mock::MockI2CController>();
    constexpr uint8_t channel = 1;
    constexpr uint16_t pos = 1;
    EXPECT_CALL(*mock_i2c, Write(PCA9685_ADDRESS, ::testing::_))
      .WillOnce(::testing::Return(srp::core::ErrorCode::kOk))
      .WillOnce(::testing::Return(srp::core::ErrorCode::kConnectionError));
    wrapper.TestSetI2C(std::move(mock_i2c));
    EXPECT_EQ(wrapper.TestSetServo(channel, pos), srp::core::ErrorCode::kOk);
    EXPECT_EQ(wrapper.TestSetServo(channel, pos), srp::core::ErrorCode::kConnectionError);
}

TEST(TestPCAController, AutoSetServoPosTest) {
    TestWrapper wrapper{};
    auto data = wrapper.read_config(FILE_PREFIX+"basic.json");
    wrapper.SetDB(data.value());
    auto mock_gpio = std::make_unique<srp::mock::MOCKGPIOController>();
    auto mock_i2c = std::make_unique<srp::mock::MockI2CController>();
    EXPECT_CALL(*mock_gpio, SetPinValue(::testing::_, ::testing::_))
      .Times(2);
    EXPECT_CALL(*mock_i2c, Write(PCA9685_ADDRESS, ::testing::_))
      .Times(2);
    wrapper.TestSetGpio(std::move(mock_gpio));
    wrapper.TestSetI2C(std::move(mock_i2c));
    EXPECT_EQ(wrapper.AutoSetServoPosition(0, 0), srp::core::ErrorCode::kNotDefine);
    EXPECT_EQ(wrapper.AutoSetServoPosition(1, 0), srp::core::ErrorCode::kError);
    EXPECT_EQ(wrapper.AutoSetServoPosition(1, 1), srp::core::ErrorCode::kOk);
}

TEST(TestPCAController, SetPtrsTest) {
    TestWrapper wrapper{};
    auto gpio = std::make_unique<srp::gpio::GPIOController>();
    auto i2c = std::make_unique<srp::i2c::I2CController>();
    EXPECT_EQ(wrapper.TestSetGpio(nullptr), srp::core::ErrorCode::kInitializeError);
    EXPECT_EQ(wrapper.TestSetGpio(std::move(gpio)), srp::core::ErrorCode::kOk);
    EXPECT_EQ(wrapper.TestSetI2C(nullptr), srp::core::ErrorCode::kInitializeError);
    EXPECT_EQ(wrapper.TestSetI2C(std::move(i2c)), srp::core::ErrorCode::kOk);
}
