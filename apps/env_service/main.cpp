/**
 * @file main.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "apps/env_service/env_service.hpp"
#include "core/application/application_factory.h"
#include "untar/untar.h"

int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::envService::EnvService>::Start(argc, argv);
  return 0;
}