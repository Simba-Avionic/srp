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
#include "ara/exec/adaptive_lifecycle.h"
#include "example_adaptive/ExampleApp2/code/application/ExampleApp.h"

int main(int argc, char const *argv[]) {
  // setsid();
  return ara::exec::RunAdaptiveLifecycle<simba::example::ExampleApp>(argc,
                                                                     argv);
}
