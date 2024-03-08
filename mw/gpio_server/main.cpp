/**
 * @file main.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "mw/gpio_server/gpio_mw.hpp"
#include "core/application/application_factory.h"

int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::mw::GPIOMWService>::Start(argc, argv);
  return 0;
}
