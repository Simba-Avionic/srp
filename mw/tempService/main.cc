
#include <iostream>
#include <thread>
#include "TempService.h"
#include "core/application/application_factory.h"

int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::mw::temp::TempApplication>::Start(argc, argv);
  return 0;
}
