/**
 * @file main.cc
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "apps/fc/radio_service/radio_app.h"
#include "ara/exec/adaptive_lifecycle.h"
int main(int argc, char const *argv[]) {
  // setsid();
  return ara::exec::RunAdaptiveLifecycle<srp::apps::RadioApp>(argc,
                                                                      argv);
}
