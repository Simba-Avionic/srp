/**
 * @file fuel_app.cpp
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "apps/ec/fuel_service/fuel_app.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"

namespace srp {
namespace apps {

FuelApp::FuelApp() {
}

int FuelApp::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "Running Fuel Application";
  core::condition::wait(token);
  ara::log::LogInfo() << "Run complete, closing";
  return 0;
}

int FuelApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
  ara::log::LogInfo() << "Initialize Complete";
  return 0;
}

}  // namespace apps
}  // namespace srp