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
