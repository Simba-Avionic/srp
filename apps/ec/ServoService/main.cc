/**
 * @file main.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ara/exec/adaptive_lifecycle.h"
#include "apps/ec/ServoService/servoService.hpp"

int main(int argc, char const *argv[]) {
  // setsid();
  return ara::exec::RunAdaptiveLifecycle<srp::service::ServoService>(argc,
                                                                     argv);
}
