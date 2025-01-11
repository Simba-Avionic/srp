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
#include "platform/common/someip_demon/code/application/someip_demon.h"

int main(int argc, char const *argv[]) {
  /* code */

  return ara::exec::RunAdaptiveLifecycle<
      srp::someip_demon::SomeIpApplication>(argc, argv);
}
