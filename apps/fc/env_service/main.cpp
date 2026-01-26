/**
 * @file main.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "apps/fc/env_service/env_service.hpp"
#include "ara/exec/adaptive_lifecycle.h"
int main(int argc, char const *argv[]) {
  // setsid();
  return ara::exec::RunAdaptiveLifecycle<srp::envService::EnvService>(argc,
                                                                     argv);
}
