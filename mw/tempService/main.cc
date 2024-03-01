
#include <iostream>
#include <thread>
#include "TempService.hpp"
#include "core/application/application_factory.h"

int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<TempApplication>::Start(argc, argv);
  return 0;
}
