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
#include "apps/engine_service/engine_app.hpp"
#include "ara/exec/adaptive_lifecycle.h"
int main(int argc, char const *argv[]) {
  return ara::exec::RunAdaptiveLifecycle<simba::apps::EngineApp>(argc,
                                                                     argv);
}
