/**
 * @file main.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "apps/ec/primer_service/app.hpp"
#include "ara/exec/adaptive_lifecycle.h"

int main(int argc, char const *argv[]) {
  return ara::exec::RunAdaptiveLifecycle<srp::primer::PrimerService>(argc,
                                                                     argv);
}
