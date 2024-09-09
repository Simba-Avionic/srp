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
#ifndef EXAMPLE_ADAPTIVE_EXAMPLEAPP2_CODE_APPLICATION_EXAMPLEAPP_H_
#define EXAMPLE_ADAPTIVE_EXAMPLEAPP2_CODE_APPLICATION_EXAMPLEAPP_H_

#include <map>

#include "ara/exec/adaptive_application.h"

namespace simba {
namespace example {
class ExampleApp : public ara::exec::AdaptiveApplication {
 private:
  /**
   * @brief This function is called to initialiaze the application
   *
   * @param parms map with parms
   */
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                     parms) override;
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  int Run(const std::stop_token& token) override;

 public:
  ExampleApp();
  ~ExampleApp();
};
}  // namespace example
}  // namespace simba
#endif  // EXAMPLE_ADAPTIVE_EXAMPLEAPP2_CODE_APPLICATION_EXAMPLEAPP_H_
