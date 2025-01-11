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
#include "platform/common/logger/code/application/dlt_service.h"

int main(int argc, char const *argv[]) {
  return ara::exec::RunAdaptiveLifecycle<srp::dlt::DltService>(argc, argv);
}
