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
#include "simba/example/ExampleServiceSkeleton.h"
#include "example_adaptive/ExampleApp/code/application/example_service.h"
namespace simba {
namespace example {
ExampleApp::ExampleApp() {}
ExampleApp::~ExampleApp() {}

int ExampleApp::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  simba::example::ExampleDataStructure struc{};
  struc.name = 10;
  //  struc.other_struct.name = -10;
  //  struc.other_struct.name = "test";
  ara::log::LogInfo() << "App Initialized";
  return 0;
}

int ExampleApp::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "App start";
  MyExampleService skeleton2{
      ara::core::InstanceSpecifier{"simba/example/ExampleApp/service2"}};
  skeleton2.StartOffer();
  uint8_t c = 0;
  while (!token.stop_requested()) {
    skeleton2.Status.Update(c++);
    core::condition::wait_for(std::chrono::seconds{1}, token);
  }
  ara::log::LogInfo() << "App Stop";
  return 0;
}

}  // namespace example
}  // namespace simba
