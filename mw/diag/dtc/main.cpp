/**
 * @file main.cpp
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "core/application/application_factory.h"
#include "mw/diag/dtc/dtcService.hpp"

int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::mw::dtc::DtcService>::Start(argc,
                                                                     argv);
  return 0;
}
