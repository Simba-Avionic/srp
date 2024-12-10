#include "ara/exec/adaptive_lifecycle.h"
#include "blinking_service.hpp"

int main(int argc, char const *argv[]) {
  return ara::exec::RunAdaptiveLifecycle<simba::envService::BlinkingService>(
      argc, argv);
}
