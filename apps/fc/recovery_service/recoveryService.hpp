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

#include "mw/i2c_service/controller/pca9685/controller.hpp"
namespace simba {
namespace service {
class RecoveryService final : public ara::exec::AdaptiveApplication {
 private:
  std::shared_ptr<i2c::PCA9685> servo_controller;
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
};

}  // namespace service
}  // namespace simba
#endif  // APPS_FC_RECOVERY_SERVICE_RECOVERYSERVICE_HPP_
