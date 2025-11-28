/**
 * @file engine_app.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "apps/ec/example_service/example_app.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"

namespace srp {
namespace apps {

ExampleApp::ExampleApp() {
}

int ExampleApp::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "Running Example Application";
  core::condition::wait(token);
  ara::log::LogInfo() << "Run complete, closing";
  return 0;
}

int ExampleApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
  ara::log::LogInfo() << "Initialize Complete";
  return 0;
}

}  // namespace apps
}  // namespace srp
