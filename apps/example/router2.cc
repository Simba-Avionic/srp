/**
 * @file router.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <memory>

#include "apps/example/router.h"
#include "communication-core/someip-controller/event_skeleton.h"
#include "communication-core/someip-controller/method_proxy.h"
#include "core/logger/Logger.h"
namespace simba {
namespace router {

core::ErrorCode Router::Run(std::stop_token token) {
  auto example =
      std::make_shared<com::someip::MethodProxyBase>("ExampleApp2/someproxy");
  auto event_example =
      std::make_shared<com::someip::EventSkeleton>("ExampleApp2/exampleEvent");
  com->Add(example);
  com->Add(event_example);
  example->StartFindService();
  event_example->SetValue({10, 11, 12, 13, 14, 15});
  while (true) {
    // this->gpio_.SetPinValue(5,gpio::Value::HIGH);
    std::ignore = example->Get();
    // this->gpio_.SetPinValue(5,gpio::Value::LOW);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode Router::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  // this->gpio_.Init(12);
  return core::ErrorCode::kOk;
}
}  // namespace router
}  // namespace simba
