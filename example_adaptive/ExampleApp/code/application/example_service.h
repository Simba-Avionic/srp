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
#include "simba/example/ExampleServiceSkeleton.h"

namespace simba {
namespace example {
class MyExampleService : public simba::example::ExampleServiceSkeleton {
 public:
  explicit MyExampleService(const ara::core::InstanceSpecifier& instance)
      : simba::example::ExampleServiceSkeleton{instance} {}
  ~MyExampleService() = default;

 protected:
  ara::core::Result<bool> SetStatus3() override { return true; }
  ara::core::Result<simba::example::ExampleDataStructure> SetStatus4()
      override {
    ara::log::LogInfo() << "Działa !!";
    simba::example::ExampleDataStructure res{1, 1};
    return res;
  }
  ara::core::Result<void> SetStatus5() override { return {}; }
};
}  // namespace example
}  // namespace simba
#endif  // EXAMPLE_ADAPTIVE_EXAMPLEAPP_CODE_APPLICATION_EXAMPLE_SERVICE_H_
