/**
 * @file mainService.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APPS_FC_MAIN_SERVICE_MAINSERVICE_HPP_
#define APPS_FC_MAIN_SERVICE_MAINSERVICE_HPP_
#include <string>
#include <unordered_map>
#include <memory>
#include <map>

#include "ara/exec/adaptive_application.h"
#include "apps/fc/main_service/service.hpp"
#include "apps/fc/main_service/rocket_state.h"
#include "core/common/error_code.h"
#include "apps/fc/main_service/rocketController.hpp"
namespace srp {
namespace service {
class MainService final : public ara::exec::AdaptiveApplication {
 private:
  std::unique_ptr<apps::MyMainServiceSkeleton> service_ipc;
  std::unique_ptr<apps::MyMainServiceSkeleton> service_udp;
  apps::RocketController controller;
 protected:
  /**
   * @brief This function is called to initialiaze the application
   *
   * @param parms map with parms
   */
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) override;
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  int Run(const std::stop_token& token) override;

 public:
  ~MainService() = default;
  MainService();
};

}  // namespace service
}  // namespace srp
#endif  // APPS_FC_MAIN_SERVICE_MAINSERVICE_HPP_
