/**
 * @file ExampleApp.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "example_adaptive/ExampleApp/code/application/ExampleApp.h"

#include <iostream>

#include "ara/log/log.h"
#include "core/common/condition.h"
#include "simba/example/ExampleDataStructure.h"
#include "example_service.h"
#include "core/uart/uart_driver.hpp"
namespace simba {
namespace example {
ExampleApp::ExampleApp() {}
ExampleApp::~ExampleApp() {}

int ExampleApp::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  ara::log::LogInfo() << "App Initialized";
  return 0;
}

int ExampleApp::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "App start";
  core::uart::UartDriver uart_;
  uart_.Open("/dev/ttyS1");
  core::uart::uart_config_t config;
  uart_.Configure(config);
  while (!token.stop_requested()) {
    core::condition::wait_for(std::chrono::milliseconds(50), token);
    if (uart_.ReadAvailable()) {
      auto res = uart_.Read();
      if (res.has_value()) {
        ara::log::LogInfo() << res.value();
      }
    }
  }
  ara::log::LogInfo() << "App Stop";
  return 0;
}

}  // namespace example
}  // namespace simba
