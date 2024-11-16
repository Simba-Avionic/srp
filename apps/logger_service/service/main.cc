/**
 * @file main.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "apps/logger_service/service/logger_service.hpp"
#include "ara/exec/adaptive_lifecycle.h"
int main(int argc, char const *argv[]) {
  // setsid();
  return ara::exec::RunAdaptiveLifecycle<simba::logger::LoggerService>(argc,
                                                                     argv);
}
