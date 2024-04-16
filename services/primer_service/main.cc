/**
 * @file main.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "services/primer_service/primer_service.hpp"
#include "core/application/application_factory.h"

int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::primer::PrimerService>::Start(argc, argv);
  return 0;
}
