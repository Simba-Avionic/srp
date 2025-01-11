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
#include "example_adaptive/ExampleApp2/code/application/ExampleApp.h"

#include <iostream>
#include <memory>

#include "ara/log/log.h"
#include "core/common/condition.h"
#include "simba/example/ExampleService/ExampleServiceHandler.h"
// #include "simba/example/ExampleDataStructure.h"
// #include "simba/example/ExampleServiceSkeleton.h"
namespace simba {
namespace example {
ExampleApp::ExampleApp() {}
ExampleApp::~ExampleApp() {}

int ExampleApp::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  //  struc.other_struct.name = -10;
  //  struc.other_struct.name = "test";
  ara::log::LogInfo() << "App Initialized";
  return 0;
}

int ExampleApp::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "App start";
  std::shared_ptr<simba::example::ExampleServiceHandler> handler_{nullptr};
  simba::example::ExampleServiceProxy proxy{
      ara::core::InstanceSpecifier{"simba/example/ExampleApp2/service_ipc_1"}};
  proxy.StartFindService([&handler_](auto handler) { handler_ = handler; });
  while (!token.stop_requested()) {
    if (handler_ != nullptr) {
      {
        const auto& res = handler_->SetStatus3();
        if (!res.HasValue()) {
          ara::log::LogError() << "1) Call method Error! :" << res.Error();
        } else {
          ara::log::LogInfo() << "1) Call method Pass";
        }
      }
      {
        const auto& res = handler_->SetStatus4();
        if (!res.HasValue()) {
          ara::log::LogError() << "2) Call method Error! :" << res.Error();
        } else {
          ara::log::LogInfo() << "2) Call method Pass";
        }
      }
      {
        const auto& res = handler_->SetStatus5();
        if (!res.HasValue()) {
          ara::log::LogError() << "3) Call method Error! :" << res.Error();
        } else {
          ara::log::LogInfo() << "3) Call method Pass";
        }
      }
    }
    core::condition::wait_for(std::chrono::seconds{5}, token);
  }

  ara::log::LogInfo() << "App Stop";
  return 0;
}

}  // namespace example
}  // namespace simba
