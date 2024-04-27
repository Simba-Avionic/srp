/**
 * @file servoService.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <memory>
#include <optional>
#include <vector>

#include "servoService.hpp"

#include "core/logger/Logger.h"

namespace simba {
namespace service {

core::ErrorCode ServoService::Run(std::stop_token token) {
  while (true) {
    // update servo positions;
    auto val = this->servo_controller.ReadServoPosition(60);
    if (val.has_value()) {
      main_servo_status_event->SetValue({val.value()});
    }
    auto val2 = this->servo_controller.ReadServoPosition(61);
    if (val2.has_value()) {
      vent_servo_status_event->SetValue({val2.value()});
    } else {
      AppLogger::Warning("FUASHUD");
    }
    AppLogger::Info("Send servo status event");
    std::this_thread::sleep_for(std::chrono::milliseconds(975));
  }
}

core::ErrorCode ServoService::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
    this->servo_controller.Init(this->app_id_, parms);
// Dodanie publikowanych eventów
    main_servo_status_event =
      std::make_shared<com::someip::EventSkeleton>("ServoApp/servoStatusEvent");
    vent_servo_status_event =
      std::make_shared<com::someip::EventSkeleton>("ServoApp/servoVentStatusEvent");

// Rejestracja serwera dla metod
auto set_servo_val = std::make_shared<com::someip::MethodSkeleton>(
    "ServoApp/setServoValue",
    [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
    if (payload.size() == 1) {
    this->servo_controller.AutoSetServoPosition(60, payload[0]);
    return std::vector<uint8_t>{};
    }
    return {};
});
auto set_vent_val = std::make_shared<com::someip::MethodSkeleton>(
    "ServoApp/setServoVentValue",
    [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
    if (payload.size() == 1) {
    this->servo_controller.AutoSetServoPosition(61, payload[0]);
    return std::vector<uint8_t>{};
    }
    return {};
});
// NOW unsupported
// auto read_servo_val = std::make_shared<com::someip::MethodSkeleton>(
//     "ServoApp/readServoValue",
//     [this](const std::vector<uint8_t> payload)
//           -> std::optional<std::vector<uint8_t>> {
//         return std::vector<uint8_t>{this->servo_controller.ReadServoPosition(60).value()};
// });
// auto read_vent_val = std::make_shared<com::someip::MethodSkeleton>(
//     "ServoApp/readServoVentValue",
//     [this](const std::vector<uint8_t> payload)
//           -> std::optional<std::vector<uint8_t>> {
//         return std::vector<uint8_t>{this->servo_controller.ReadServoPosition(61).value()};
// });


// Rejestracja Metod i eventow
com->Add(main_servo_status_event);
com->Add(vent_servo_status_event);
com->Add(set_servo_val);
com->Add(set_vent_val);
// UNSUPPORTED
// com->Add(read_servo_val);
// com->Add(read_vent_val);

return core::ErrorCode::kOk;
}

}  // namespace service
}  // namespace simba
