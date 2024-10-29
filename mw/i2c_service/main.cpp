/**
 * @file main.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "mw/i2c_service/service/i2c_service.h"
#include "ara/exec/adaptive_lifecycle.h"

int main(int argc, char const *argv[]) {
  // setsid();
  return ara::exec::RunAdaptiveLifecycle<simba::mw::I2CService>(argc,
                                                                     argv);
}