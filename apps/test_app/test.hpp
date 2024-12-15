#ifndef APPS_TEST_APP_HPP
#define APPS_TEST_APP_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <memory>

#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "ara/exec/adaptive_application.h"

namespace simba {
  namespace testApp {

    class TestApp final : public ara::exec::AdaptiveApplication {
    private:
      gpio::GPIOController gpio_;

      int Run(const std::stop_token& token) override;
      int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
        parms) override;

    public:
      ~TestApp() = default;
      TestApp();
    };

  }  // namespace TestApp
}  // namespace simba


#endif  // APPS_TEST_APP_HPP
