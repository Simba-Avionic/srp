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
#include "platform/common/em/code/em_application.h"

int main(int argc, char const *argv[]) {
  /* code */

  return ara::exec::RunAdaptiveLifecycleWithoutControl<
      srp::em::EmApplication>(argc, argv);
}
