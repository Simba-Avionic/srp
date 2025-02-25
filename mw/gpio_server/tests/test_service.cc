/**
 * @file test_service.cc
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief Unit tests for gpio service
 * @version 0.1
 * @date 2024-06-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <memory>
#include "gtest/gtest.h"

#include "mw/gpio_server/data/header.hpp"
#include "mw/gpio_server/gpio_mw.hpp"
#include "core/gpio/mock/mock_gpio_driver.h"
#include "communication-core/sockets/mock/mockSocket.h"

namespace {
    static const std::string FILE_PREFIX = "mw/gpio_server/tests/test_data/";
}

class TestWrapper : public srp::mw::GPIOMWService {
 public:
  int TestInit(std::unique_ptr<MockStreamSocket> socket,
    std::unique_ptr<MockGPIO> gpio) {
      return this->Init(std::move(socket), std::move(gpio));
  }
  std::optional<std::unordered_map<uint8_t, srp::mw::GpioConf>> TestReadConfig(std::string path) {
    return this->ReadConfig(path);
  }
  void SetConfig(const std::unordered_map<uint8_t, srp::mw::GpioConf>& config) {
    this->config = config;
  }
  int TestInitPins() {
    return this->InitPins();
  }
  std::vector<uint8_t> TestRxCallback(const std::vector<std::uint8_t>& data) {
    return this->RxCallback("", 0, data);
  }
};

// class ReadConfigTest : public ::testing::TestWithParam<
//   std::tuple<std::string, std::optional<std::unordered_map<uint8_t, srp::mw::GpioConf>>>> {
//  protected:
//   std::string path;
//   std::optional<std::unordered_map<uint8_t, srp::mw::GpioConf>> db_opt;

//   void SetUp() override {
//     path = std::get<0>(GetParam());
//     db_opt = std::get<1>(GetParam());
//   }
// };

// TEST_P(ReadConfigTest, TEST_READ_CONFIG) {
//     TestWrapper wrapper{};
//     auto res_opt = wrapper.TestReadConfig(FILE_PREFIX + path);
//     ASSERT_EQ(db_opt.has_value(), res_opt.has_value());
//     if (db_opt.has_value()) {
//         auto db = db_opt.value();
//         auto res = res_opt.value();
//         ASSERT_EQ(db.size(), res.size());
//         for (const auto& instance : db) {
//             EXPECT_EQ(instance.second.pinNum, res[instance.first].pinNum);
//             EXPECT_EQ(instance.second.direction, res[instance.first].direction);
//         }
//     } else {
//         EXPECT_FALSE(res_opt.has_value());
//     }
// }


// INSTANTIATE_TEST_SUITE_P(ReadConfigTestParameters, ReadConfigTest,
//   ::testing::Values(
//     std::make_tuple("t1.json", std::unordered_map<uint8_t, srp::mw::GpioConf>
//         {{{1, {21, srp::core::gpio::direction_t::OUT}}}}),
//     std::make_tuple("t2.json", std::nullopt),
//     std::make_tuple("t3.json", std::optional<std::unordered_map<uint8_t, srp::mw::GpioConf>>{std::in_place}),
//     std::make_tuple("t4.json", std::optional<std::unordered_map<uint8_t, srp::mw::GpioConf>>{std::in_place}),
//     std::make_tuple("t5.json", std::optional<std::unordered_map<uint8_t, srp::mw::GpioConf>>{std::in_place}),
//     std::make_tuple("t6.json", std::unordered_map<uint8_t, srp::mw::GpioConf>
//                                                         {{{1, {21, srp::core::gpio::direction_t::OUT}}}})));



TEST(GPIO_SERVICE, TEST_INIT_PINS) {
    TestWrapper wrapper{};
    auto mock_socket = std::make_unique<MockStreamSocket>();
    auto mock_gpio_driver = std::make_unique<MockGPIO>();

    EXPECT_CALL(*mock_gpio_driver, initializePin(::testing::_, ::testing::_))
        .WillOnce(::testing::Return(srp::core::ErrorCode::kOk));

    std::unordered_map<uint8_t, srp::mw::GpioConf> conf{
        {1, {21, srp::core::gpio::direction_t::OUT}}
    };

    wrapper.SetConfig(conf);
    wrapper.TestInit(std::move(mock_socket), std::move(mock_gpio_driver));

    EXPECT_EQ(wrapper.TestInitPins(), srp::core::ErrorCode::kOk);
}

// class RXCallbackTest : public ::testing::TestWithParam<
// std::tuple<uint16_t,  uint8_t, srp::gpio::ACTION, std::unordered_map<uint8_t,
//     srp::mw::GpioConf>, std::vector<uint8_t>>> {
// };

// INSTANTIATE_TEST_SUITE_P(RXCallbackTestParameters, RXCallbackTest,
//  ::testing::Values(
//   std::make_tuple(1, 0, srp::gpio::ACTION::SET, std::unordered_map<uint8_t, srp::mw::GpioConf>
//       {{1, {21, srp::core::gpio::direction_t::OUT}}}, std::vector<uint8_t>{1}),

//   std::make_tuple(1, 0, srp::gpio::ACTION::SET, std::unordered_map<uint8_t, srp::mw::GpioConf>
//       {{1, {21, srp::core::gpio::direction_t::IN}}}, std::vector<uint8_t>{0}),

//   std::make_tuple(1, 0, srp::gpio::ACTION::GET, std::unordered_map<uint8_t, srp::mw::GpioConf>
//       {{1, {21, srp::core::gpio::direction_t::OUT}}}, std::vector<uint8_t>{0}),

//   std::make_tuple(1, 0, srp::gpio::ACTION::RES, std::unordered_map<uint8_t, srp::mw::GpioConf>
//       {{1, {21, srp::core::gpio::direction_t::OUT}}}, std::vector<uint8_t>{}),

//   std::make_tuple(1, 0, srp::gpio::ACTION::RES, std::unordered_map<uint8_t, srp::mw::GpioConf>
//       {}, std::vector<uint8_t>{0})
// ));

// TEST_P(RXCallbackTest, RXCALLBACK_TEST) {
//     TestWrapper wrapper{};
//     auto params = GetParam();
//     auto mock_socket = std::make_unique<MockStreamSocket>();
//     auto mock_gpio_driver = std::make_unique<MockGPIO>();
//     std::cout<< "dupa1" <<std::endl;

//     const uint8_t actuatorID = std::get<0>(params);
//     const uint8_t value = std::get<1>(params);
//     const srp::gpio::ACTION action = std::get<2>(params);
//     auto config = std::get<3>(params);
//     std::vector<uint8_t> result = std::get<4>(params);

//     srp::gpio::Header hdr{actuatorID, value, action};
//     auto data = hdr.GetBuffor();
//     std::cout<< "dupa2" <<std::endl;
//     if (config.find(actuatorID) != config.end()) {
//       if (action == srp::gpio::ACTION::SET &&
//              config[actuatorID].direction == srp::core::gpio::direction_t::OUT) {
//         std::cout<< "dupa3" <<std::endl;
//         EXPECT_CALL(*mock_gpio_driver, setValue(::testing::_, ::testing::_))
//           .WillOnce(::testing::Return(srp::core::ErrorCode::kOk))
//           .WillOnce(::testing::Return(srp::core::ErrorCode::kError));
// std::cout<< "dupa4" <<std::endl;
//         wrapper.TestInit(std::move(mock_socket), std::move(mock_gpio_driver));
//         wrapper.SetConfig(config);
//         std::cout<< "dupa5" <<std::endl;
//         EXPECT_EQ(wrapper.TestRxCallback(data), result);
//         std::cout<< "dupa55" <<std::endl;
//         EXPECT_EQ(wrapper.TestRxCallback(data)[0], 0);
//         std::cout<< "dupa6" <<std::endl;
//       }
//       if (action == srp::gpio::ACTION::GET) {
//         std::cout<< "dupa7" <<std::endl;
//         EXPECT_CALL(*mock_gpio_driver, getValue(::testing::_))
//           .WillOnce(::testing::Return(0))
//           .WillOnce(::testing::Return(1))
//           .WillOnce(::testing::Return(2));
// std::cout<< "dupa8" <<std::endl;
//         std::vector<uint8_t> responses = {0, 1, 2};
//         wrapper.TestInit(std::move(mock_socket), std::move(mock_gpio_driver));
//         wrapper.SetConfig(config);
// std::cout<< "dupa9" <<std::endl;
//         for (const auto& response : responses) {
//           auto Callback = wrapper.TestRxCallback(data);
//           srp::gpio::Header hdr2(0, 0, srp::gpio::ACTION::GET);
//           hdr2.SetBuffor(Callback);
//           EXPECT_EQ(hdr2.GetAction(), srp::gpio::ACTION::RES);
//           EXPECT_EQ(hdr2.GetActuatorID(), actuatorID);
//           EXPECT_EQ(hdr2.GetValue(), response);
//           std::cout<< "dupa10" <<std::endl;
//         }
//       }
//       if (action == srp::gpio::ACTION::RES) {
//         wrapper.TestInit(std::move(mock_socket), std::move(mock_gpio_driver));
//         wrapper.SetConfig(config);
//         EXPECT_EQ(wrapper.TestRxCallback(data), result);
//       }
//     } else {
//       EXPECT_EQ(wrapper.TestRxCallback(data), result);
//     }
// }
