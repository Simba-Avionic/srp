/**
 * @file main.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "servoService.hpp"
#include "core/application/application_factory.h"
#include "untar/untar.h"

int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::service::ServoService>::Start(argc, argv);
  return 0;
}
