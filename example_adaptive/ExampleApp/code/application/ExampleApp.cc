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
  core::condition::wait(token);
  ara::log::LogInfo() << "App Stop";
  return 0;
}

}  // namespace example
}  // namespace simba
