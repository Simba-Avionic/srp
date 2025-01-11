/**
 * @file main.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ara/exec/adaptive_lifecycle.h"
#include "platform/common/state_manager/code/state_manager.h"
int main(int argc, char const *argv[]) {
  return ara::exec::RunAdaptiveLifecycle<srp::sm::StateManager>(argc, argv);
}
