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
    main_servo_status_event->SetValue({0x12, 0x15});
    vent_servo_status_event->SetValue({0x11, 0x15});
    std::this_thread::sleep_for(std::chrono::milliseconds(950));
}
}

core::ErrorCode ServoService::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
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
        return std::vector<uint8_t>{0, 1, 2};
});
auto set_vent_val = std::make_shared<com::someip::MethodSkeleton>(
    "ServoApp/setServoVentValue",
    [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
        return std::vector<uint8_t>{0, 1, 2};
});
// NOW unsupported
auto read_servo_val = std::make_shared<com::someip::MethodSkeleton>(
    "ServoApp/readServoValue",
    [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
        return std::vector<uint8_t>{0, 1, 2};
});
auto read_vent_val = std::make_shared<com::someip::MethodSkeleton>(
    "ServoApp/readServoVentValue",
    [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
        return std::vector<uint8_t>{0, 1, 2};
});

// Rejestracja Metod i eventow
com->Add(main_servo_status_event);
com->Add(vent_servo_status_event);
com->Add(set_servo_val);
com->Add(set_vent_val);
// UNSUPPORTED
com->Add(read_servo_val);
com->Add(read_vent_val);

return core::ErrorCode::kOk;
}

}  // namespace service
}  // namespace simba
