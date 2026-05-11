/**
 * @file main.cc
 * @author Wiktor Laska   
 * @brief 
 * @version 0.1
 * @date 2025-04-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "apps/fc/apogee_service/apogee_app.hpp"
#include "ara/exec/adaptive_lifecycle.h"
int main(int argc, char const *argv[]) {
  return ara::exec::RunAdaptiveLifecycle<srp::apps::ApogeeService>(argc,
                                                                      argv);
}
