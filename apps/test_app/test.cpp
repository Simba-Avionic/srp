#include "apps/test_app/test.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"

namespace simba {
  namespace testApp {

    int TestApp::Run(const std::stop_token& token) {
      core::condition::wait(token);
      return 0;
    }
    int TestApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView> parms) {
      if (this->gpio_.SetPinValue(1, 1) != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to turn on LED 1";
      }

      if (this->gpio_.SetPinValue(2, 1) != core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to turn on LED 2";
      }

      return 0;
    }

    TestApp::TestApp() : gpio_(std::make_unique<com::soc::StreamIpcSocket>()) {}

  }  // namespace testApp
}  // namespace simba