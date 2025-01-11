/**
 * @file main.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <thread>  // NOLINT

#include "ara/exec/adaptive_lifecycle.h"
#include "example_adaptive/ExampleApp/code/application/ExampleApp.h"

int main(int argc, char const *argv[]) {
  std::this_thread::sleep_for(std::chrono::seconds{2});
  return ara::exec::RunAdaptiveLifecycle<simba::example::ExampleApp>(argc,
                                                                     argv);
}
