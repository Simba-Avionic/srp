#include "core/applicationFactory.h"
#include "apps/homework/exampleApp.h"
int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::example::exampleApp>::Start(argc, argv);
  return 0;
}