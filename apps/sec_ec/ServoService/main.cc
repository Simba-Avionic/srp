/**
 * @file main.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief Entry point for secondary EC Servo Service
 * @version 0.1
 * @date 2026-07-01
 *
 * @copyright Copyright (c) 2026
 */
#include "ara/exec/adaptive_lifecycle.h"
#include "apps/sec_ec/ServoService/servoService.hpp"

int main(int argc, char const *argv[]) {
  return ara::exec::RunAdaptiveLifecycle<srp::sec_ec::SecServoService>(argc,
                                                                       argv);
}
