/**
 * @file parachute_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "apps/fc/recovery_service/parachute_controller.hpp"
#include <utility>
namespace srp {
namespace apps {
namespace recovery {
namespace {
  constexpr auto kLinecutter_pin_id = 1;
  constexpr auto kRecovery_servo_id = 63;
  constexpr auto kServo_move_time = 500;
  constexpr auto kServo_sequence_num = 3;
  constexpr auto kLinecutter_sequence_num = 3;
  constexpr auto kLinecutter_active_time = 3000;
  constexpr auto kLinecutter_inactive_time = 1000;
}
void ParachuteController::Init(std::unique_ptr<i2c::PCA9685>&& servo, std::unique_ptr<gpio::GPIOController>&& gpio) {
    this->servo_controller = std::move(servo);
    this->gpio_controller = std::move(gpio);
}

bool ParachuteController::OpenParachute(bool diag) {
    if (!servo_controller) {
        return false;
    }
    if (parachute_open && !diag) {
      return false;
    }
    if (!diag) {
      parachute_open = true;
    }
    for (uint8_t i = 0; i < kServo_sequence_num; i++) {
      this->servo_controller->AutoSetServoPosition(kRecovery_servo_id, 1);
      std::this_thread::sleep_for(std::chrono::milliseconds(kServo_move_time));
      this->servo_controller->AutoSetServoPosition(kRecovery_servo_id, 0);
      std::this_thread::sleep_for(std::chrono::milliseconds(kServo_move_time));
    }
    return true;
}
bool ParachuteController::UnreefParachute(bool diag) {
    if (!gpio_controller) {
        return false;
    }
    if ((parachute_unreefed || !parachute_open) && !diag) {
      return true;
    }
    if (!diag) {
      parachute_unreefed = true;
    }
    for (auto i = 0; i < kLinecutter_sequence_num; i++) {
      this->gpio_controller->SetPinValue(kLinecutter_pin_id, 1);
      std::this_thread::sleep_for(std::chrono::milliseconds(kLinecutter_active_time));
      this->gpio_controller->SetPinValue(kLinecutter_pin_id, 0);
      std::this_thread::sleep_for(std::chrono::milliseconds(kLinecutter_inactive_time));
    }
    return true;
}
ParachuteController::ParachuteController(): parachute_unreefed(false), parachute_open(false) {
}


}  // namespace recovery
}  // namespace apps
}  // namespace srp
