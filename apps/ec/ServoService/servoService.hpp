/**
 * @file servoService.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_EC_SERVOSERVICE_SERVOSERVICE_HPP_
#define APPS_EC_SERVOSERVICE_SERVOSERVICE_HPP_
#include <string>
#include <unordered_map>
#include <memory>
#include <map>

#include "ara/exec/adaptive_application.h"

#include "mw/i2c_service/controller/pca9685/controller.hpp"
<<<<<<< HEAD:apps/ec/ServoService/servoService.hpp
#include "apps/ec/ServoService/servo_service_did.h"
#include "apps/ec/ServoService/service.hpp"
=======
#include "apps/ServoService/servo_service_did.h"
#include "apps/ServoService/servo_did.h"
#include "apps/ServoService/service.hpp"
>>>>>>> 6e1dfd2 (The changes made to the code are related to the `PCA9685` class in the `i2c` namespace. The main changes are:):apps/ServoService/servoService.hpp
namespace simba {
namespace service {
class ServoService final : public ara::exec::AdaptiveApplication {
 private:
  std::shared_ptr<i2c::PCA9685> servo_controller;
  std::unique_ptr<ServoServiceDiD> main_servo_service_did_;
  std::unique_ptr<ServoServiceDiD> vent_servo_service_did_;
  std::unique_ptr<ServoSecondDid> servo_did_;
  std::unique_ptr<apps::MyServoService> service_ipc;
  std::unique_ptr<apps::MyServoService> service_udp;

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
  ~ServoService() = default;
};

}  // namespace service
}  // namespace simba
#endif  // APPS_EC_SERVOSERVICE_SERVOSERVICE_HPP_
