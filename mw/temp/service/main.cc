/**
 * @file main.cc
 * @author Maciek Matuszewski (maciej.matuszewsky@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "temp_service.h"
#include "core/application/application_factory.h"

int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::mw::temp::TempService>::Start(argc, argv);
  return 0;
}
