#include "apps/example/router.h"
#include "core/application/application_factory.h"
#include "untar/untar.h"

int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::router::Router>::Start(argc, argv);
  return 0;
}
