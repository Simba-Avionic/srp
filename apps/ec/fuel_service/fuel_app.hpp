/**
 * @file fuel_app.hpp
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef APPS_EC_FUEL_SERVICE_FUEL_APP_HPP_
#define APPS_EC_FUEL_SERVICE_FUEL_APP_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <memory>

#include "ara/exec/adaptive_application.h"

namespace srp {
namespace apps {
class FuelApp final : public ara::exec::AdaptiveApplication {
 protected:
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  int Run(const std::stop_token& token) override;
  /**
   * @brief This function is called to initialiaze the application
   *
   * @param parms map with parms
   */
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) override;

 public:
    FuelApp();
};

}  // namespace apps
}  // namespace srp


#endif  // APPS_EC_FUEL_SERVICE_FUEL_APP_HPP_