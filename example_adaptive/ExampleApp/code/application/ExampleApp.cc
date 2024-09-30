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
#include "example_adaptive/ExampleApp/code/application/example_service.h"
#include "simba/example/ExampleDataStructure.h"
#include "simba/example/ExampleService/ExampleServiceHandler.h"
#include "simba/example/ExampleServiceSkeleton.h"
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
  // skeleton2.StartOffer();
  uint8_t c = 0;
  auto service_proxy = simba::example::ExampleServiceProxy(
      ara::core::InstanceSpecifier{"simba/example/ExampleApp/service3"});
  service_proxy.StartFindService([](auto handler) {
    ara::log::LogInfo() << "simba/example/ExampleApp/service3 <- found";
    handler->SetStatus3();
    ara::log::LogInfo() << "simba/example/ExampleApp/service3 <- callback done";
    handler->SetStatus3();
    ara::log::LogInfo() << "simba/example/ExampleApp/service3 <- callback done";
    handler->SetStatus3();
    ara::log::LogInfo() << "simba/example/ExampleApp/service3 <- callback done";
    handler->SetStatus3();
    ara::log::LogInfo() << "simba/example/ExampleApp/service3 <- callback done";
    handler->SetStatus3();
    ara::log::LogInfo() << "simba/example/ExampleApp/service3 <- callback done";
    handler->SetStatus3();
    ara::log::LogInfo() << "simba/example/ExampleApp/service3 <- callback done";
    handler->SetStatus3();
    ara::log::LogInfo() << "simba/example/ExampleApp/service3 <- callback done";
  });

  // auto proxy =
  // simba::example::ExampleServiceProxy(ara::core::InstanceSpecifier{"simba/example/ExampleApp/service_ipc"});
  // proxy.StartFindService([](auto handler) {
  //     ara::log::LogInfo() << "Try Subscribe to service";
  //     handler->Status.Subscribe(1, [&handler](const uint8_t& status) {
  //       ara::log::LogInfo() << "New status for my event !!! (" << status <<
  //       ")"; handler->Status.SetReceiveHandler([&handler]() {
  //         const auto val = handler->Status.GetNewSamples();
  //         if (val.HasValue()) {
  //           ara::log::LogInfo()
  //               << "New value for my event !!! (" << val.Value() << ")";
  //         } else {
  //           ara::log::LogError()
  //               << "Error with getting new value: " << val.Error();
  //         }
  //       });
  //     });
  //   });

  ara::log::LogInfo() << "App started";
  while (!token.stop_requested()) {
    // skeleton2.Status.Update(c++);
    core::condition::wait_for(std::chrono::seconds{1}, token);
  }
  ara::log::LogInfo() << "App Stop";
  return 0;
}

}  // namespace example
}  // namespace simba
