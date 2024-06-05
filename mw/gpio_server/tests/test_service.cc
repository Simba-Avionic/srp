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

#include "mw/gpio_server/gpio_mw.hpp"
#include "core/gpio/mock/mock_gpio_driver.h"
#include "communication-core/sockets/mock/mockSocket.h"

class TestWrapper : public simba::mw::GPIOMWService {
 public:
  simba::core::ErrorCode TestInit(std::unique_ptr<MockStreamSocket> socket,
    std::unique_ptr<MockGPIO> gpio) {
      return this->Init(std::move(socket), std::move(gpio));
  }
  std::optional<std::unordered_map<uint16_t, simba::mw::GpioConf>> TestReadConfig(nlohmann::json data) {
    return this->ReadConfig(data);
  }
  void SetConfig(const std::unordered_map<uint16_t, simba::mw::GpioConf>& config) {
    std::cout<<"1"<<std::endl;
    this->config = config;
    std::cout<<this->config.size()<<std::endl;
  }
  simba::core::ErrorCode TestInitPins() {
    return this->InitPins();
  }
};

class ReadConfigTest : public ::testing::TestWithParam<
  std::tuple<std::string, std::optional<std::unordered_map<uint16_t, simba::mw::GpioConf>>>> {
};

INSTANTIATE_TEST_SUITE_P(ReadConfigTestParameters, ReadConfigTest,
  ::testing::Values(
    std::make_tuple(R"({
    "gpio":[
        {
            "id":1,
            "num":21,
            "direction":"out",
            "desc":"L0"
        }
        ]})", std::optional<std::unordered_map<uint16_t, simba::mw::GpioConf>>
        {{{1, {21, simba::core::gpio::direction_t::OUT}}}}),
    std::make_tuple("", std::optional<std::unordered_map<uint16_t, simba::mw::GpioConf>>{})
    // std::make_tuple(R"({
    // "gpio":[
    //     {
    //         "num":21,
    //         "direction":"out",
    //         "desc":"L0"
    //     }
    //     ]})", std::optional<std::unordered_map<uint16_t, simba::mw::GpioConf>>{}),
    // std::make_tuple(R"({
    // "gpio":[
    //     {   
    //         "id": 1,
    //         "direction":"out",
    //         "desc":"L0"
    //     }
    //     ]})", std::optional<std::unordered_map<uint16_t, simba::mw::GpioConf>>{}),
    // std::make_tuple(R"({
    // "gpio":[
    //     {   
    //         "id": 1,
    //         "num": 21,
    //         "desc":"L0"
    //     }
    //     ]})", std::optional<std::unordered_map<uint16_t, simba::mw::GpioConf>>{}),
    // std::make_tuple(R"({
    // "gpio":[
    //     {   
    //         "id": 1,
    //         "num": 21,
    //         "direction":"out"
    //     }
    //     ]})", std::optional<std::unordered_map<uint16_t, simba::mw::GpioConf>>
    //     {{{1, {21, simba::core::gpio::direction_t::OUT}}}})
        ));


TEST_P(ReadConfigTest, TEST_READ_CONFIG) {
  TestWrapper wrapper{};
  auto params = GetParam();
  std::string json = std::get<0>(params);
  auto optional_db = std::get<1>(params);

  if (nlohmann::json::accept(json)) {
    nlohmann::json config = nlohmann::json::parse(json);
    auto optional_map = wrapper.TestReadConfig(config);
    EXPECT_EQ(optional_db.has_value(), optional_map.has_value());

     if (optional_map.has_value()) {
      auto db = optional_db.value();
      auto config_map = optional_map.value();
      ASSERT_EQ(config_map.size(), db.size());

      for (const auto& instance : config_map) {
        EXPECT_EQ(instance.second.pinNum, db[instance.first].pinNum);
        EXPECT_EQ(instance.second.direction, db[instance.first].direction);
       }
     }
  } else {
      EXPECT_EQ(optional_db.has_value(), 0);
  }
}


TEST(GPIO_SERVICE, TEST_INIT_PINS) {
    TestWrapper wrapper{};
    auto mock_socket = std::make_unique<MockStreamSocket>();
    auto mock_gpio_driver = std::make_unique<MockGPIO>();

    EXPECT_CALL(*mock_gpio_driver, initializePin(::testing::_, ::testing::_))
        .WillOnce(::testing::Return(simba::core::ErrorCode::kOk));

    std::unordered_map<uint16_t, simba::mw::GpioConf> conf{
        {1, {21, simba::core::gpio::direction_t::OUT}}
    };

    wrapper.SetConfig(conf);
    wrapper.TestInit(std::move(mock_socket), std::move(mock_gpio_driver));

    EXPECT_EQ(wrapper.TestInitPins(), simba::core::ErrorCode::kOk);
}