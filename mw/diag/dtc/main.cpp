/**
 * @file main.cpp
 * @author Mateusz Krajewski (matikrajek42@gmaill.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "core/application/application_factory.h"
#include "mw/diag/dtc/service/dtcService.h"

int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::mw::dtc::DtcService>::Start(argc,
                                                                     argv);
  return 0;
}
