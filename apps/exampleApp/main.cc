#include "core/application/application_factory.h"
#include "apps/exampleApp/exampleApp.h"
int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::example::exampleApp>::Start(argc, argv);
  return 0;
}