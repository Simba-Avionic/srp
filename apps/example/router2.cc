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
  auto example2 =
      std::make_shared<com::someip::MethodProxyBase>("ExampleApp2/someproxy2");

  com->Add(example);
  com->Add(example2);
  example->StartFindService();
  example2->StartFindService();
  while (true) {
    // this->gpio_.SetPinValue(5,gpio::Value::HIGH);
    std::ignore = example->Get();
    std::ignore = example2->Get();
    // this->gpio_.SetPinValue(5,gpio::Value::LOW);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode Router::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  return core::ErrorCode::kOk;
}
}  // namespace router
}  // namespace simba
