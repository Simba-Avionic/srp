/**
 * @file recoveryService.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_FC_RECOVERY_SERVICE_RECOVERYSERVICE_HPP_
#define APPS_FC_RECOVERY_SERVICE_RECOVERYSERVICE_HPP_
#include <string>
#include <unordered_map>
#include <memory>
#include <map>

#include "ara/exec/adaptive_application.h"
#include "apps/fc/recovery_service/service.hpp"
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "apps/fc/recovery_service/parachute_controller.hpp"
#include "apps/fc/recovery_service/recovery_rid.hpp"
#include "core/timestamp/timestamp_driver.hpp"
namespace srp {
namespace service {
class RecoveryService final : public ara::exec::AdaptiveApplication {
 private:
  const ara::core::InstanceSpecifier rec_did_specifier;
  std::shared_ptr<apps::recovery::ParachuteController> controller;
  std::unique_ptr<apps::MyRecoveryServiceSkeleton> service_ipc;
  std::unique_ptr<apps::MyRecoveryServiceSkeleton> service_udp;
  std::unique_ptr<apps::RecoveryGenericRoutine> rec_did;

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
  ~RecoveryService() = default;
  RecoveryService();
};

}  // namespace service
}  // namespace srp
#endif  // APPS_FC_RECOVERY_SERVICE_RECOVERYSERVICE_HPP_
