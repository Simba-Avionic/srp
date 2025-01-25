/**
 * @file ExampleApp.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef EXAMPLE_ADAPTIVE_EXAMPLEAPP_CODE_APPLICATION_EXAMPLE_SERVICE_H_
#define EXAMPLE_ADAPTIVE_EXAMPLEAPP_CODE_APPLICATION_EXAMPLE_SERVICE_H_
#include <map>

#include "ara/log/log.h"
#include "srp/example/ExampleServiceSkeleton.h"

namespace srp {
namespace example {
class MyExampleService : public srp::example::ExampleServiceSkeleton {
 public:
  explicit MyExampleService(const ara::core::InstanceSpecifier& instance)
      : srp::example::ExampleServiceSkeleton{instance} {}
  ~MyExampleService() = default;

 protected:
  ara::core::Result<bool> SetStatus3() override { return true; }
  ara::core::Result<srp::example::ExampleDataStructure> SetStatus4()
      override {
    ara::log::LogInfo() << "Działa !!";
    srp::example::ExampleDataStructure res{1, 1};
    return res;
  }
  ara::core::Result<void> SetStatus5() override { return {}; }
};
}  // namespace example
}  // namespace srp
#endif  // EXAMPLE_ADAPTIVE_EXAMPLEAPP_CODE_APPLICATION_EXAMPLE_SERVICE_H_
