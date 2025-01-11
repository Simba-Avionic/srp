/**
 * @file main.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-30
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "mw/em/code/em_application.h"
#include "core/application/application_factory.h"

int main(int argc, char const *argv[]) {
  srp::core::ApplicationFactory<srp::em::EmApplication>::Start(argc, argv);
  return 0;
}
