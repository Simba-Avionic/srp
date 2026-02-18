/**
 * @file main.cpp
 * @author Grzegorz Horbaczewski (gregority9@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "apps/fc/env_service/env_service.hpp"
#include "ara/exec/adaptive_lifecycle.h"
int main(int argc, char const *argv[]) {
  // setsid();
  return ara::exec::RunAdaptiveLifecycle<srp::envServiceFc::EnvServiceFc>(argc,
                                                                     argv);
}
