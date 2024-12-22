/**
 * @file parachute_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_FC_RECOVERY_SERVICE_PARACHUTE_CONTROLLER_HPP_
#define APPS_FC_RECOVERY_SERVICE_PARACHUTE_CONTROLLER_HPP_
#include <memory>
#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "mw/gpio_server/controller/gpio_controller.hpp"
namespace simba {
namespace apps {
namespace recovery {

class ParachuteController {
 private:
  std::unique_ptr<i2c::PCA9685> servo_controller;
  std::unique_ptr<gpio::GPIOController> gpio_controller;
  bool parachute_unreefed;
  bool parachute_open;
 public:
  bool OpenParachute(bool diag = false);
  bool UnreefParachute(bool diag = false);
  void Init(std::unique_ptr<i2c::PCA9685>&& servo, std::unique_ptr<gpio::GPIOController>&& gpio);
  ParachuteController();
};

}  // namespace recovery
}  // namespace apps
}  // namespace simba

#endif  // APPS_FC_RECOVERY_SERVICE_PARACHUTE_CONTROLLER_HPP_
