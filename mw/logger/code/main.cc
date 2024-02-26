/**
 * @file main.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "core/application/application_factory.h"
#include "mw/logger/code/application/dlt_service.h"
int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::mw::dlt::DltService>::Start(argc,
                                                                     argv);
  return 0;
}
