
#include <iostream>
#include <thread>
#include "TempController.h"
#include "core/application/application_factory.h"

int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::mw::temp::TempController>::Start(argc, argv);
  return 0;
}
