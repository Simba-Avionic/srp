/**
 * @file main.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "apps/ec/example_service/example_app.hpp"
#include "ara/exec/adaptive_lifecycle.h"
int main(int argc, char const *argv[]) {
  return ara::exec::RunAdaptiveLifecycle<srp::apps::ExampleApp>(argc,
                                                                     argv);
}
