#include "apps/test_app/test.hpp"
#include "ara/exec/adaptive_lifecycle.h"

int main(int argc, char const* argv[]) {
    return ara::exec::RunAdaptiveLifecycle<simba::testApp::TestApp>(argc, argv);
}
