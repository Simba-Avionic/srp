/**
 * @file main.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief Entry point for secondary EC environment service
 * @version 0.1
 * @date 2026-07-01
 *
 * @copyright Copyright (c) 2026
 */
#include "apps/sec_ec/env_service/env_service.hpp"
#include "ara/exec/adaptive_lifecycle.h"

int main(int argc, char const *argv[]) {
  return ara::exec::RunAdaptiveLifecycle<srp::sec_ec::SecEnvService>(argc, argv);
}
