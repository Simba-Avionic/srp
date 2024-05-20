/**
 * @file main.cc
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "apps/homework/homework_app.h"
#include "core/application/application_factory.h"

int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::Homework>::Start(argc, argv);
  return 0;
}
