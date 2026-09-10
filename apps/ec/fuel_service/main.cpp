/**
 * @file main.cpp
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "apps/ec/fuel_service/fuel_app.hpp"
#include "ara/exec/adaptive_lifecycle.h"
int main(int argc, char const *argv[]) {
  return ara::exec::RunAdaptiveLifecycle<srp::apps::FuelApp>(argc, argv);
}