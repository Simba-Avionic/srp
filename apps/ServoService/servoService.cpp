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
    }
    AppLogger::Info("Send servo status event");
    std::this_thread::sleep_for(std::chrono::milliseconds(975));
  }
}

core::ErrorCode ServoService::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
    //this->servo_controller.Init(parms, std::make_unique<i2c::I2CController>());
    this->servo_controller.Init(parms);
    main_servo_status_event =
      std::make_shared<com::someip::EventSkeleton>("ServoApp/servoStatusEvent");
    vent_servo_status_event =
      std::make_shared<com::someip::EventSkeleton>("ServoApp/servoVentStatusEvent");
  this->set_servo_val = std::make_shared<com::someip::MethodSkeleton>(
    "ServoApp/setServoValue",
    [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
    if (payload.size() == 1) {
    AppLogger::Debug("move servo id: 60 to pos"+std::to_string(static_cast<int>(payload[0])));
    auto res = this->servo_controller.AutoSetServoPosition(60, payload[0]);
    if (res != core::ErrorCode::kOk) {
      return std::vector<uint8_t>{0};
    }
    return std::vector<uint8_t>{1};
    }
    return {};
});
this->set_vent_val = std::make_shared<com::someip::MethodSkeleton>(
    "ServoApp/setServoVentValue",
    [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
    if (payload.size() == 1) {
    AppLogger::Debug("move servo id: 61 to pos"+std::to_string(static_cast<int>(payload[0])));
    auto res = this->servo_controller.AutoSetServoPosition(61, payload[0]);
    if (res != core::ErrorCode::kOk) {
      return std::vector<uint8_t>{0};
    }
    return std::vector<uint8_t>{1};
    }
    return {};
});

auto read_servo_val = std::make_shared<com::someip::MethodSkeleton>(
    "ServoApp/readServoValue",
    [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
        auto val = this->servo_controller.ReadServoPosition(60);
        if (!val.has_value()) {
          return {};
        }
        return std::vector<uint8_t>{val.value()};
});
auto read_vent_val = std::make_shared<com::someip::MethodSkeleton>(
    "ServoApp/readServoVentValue",
    [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
        auto val = this->servo_controller.ReadServoPosition(61);
        if (!val.has_value()) {
          return {};
        }
        return std::vector<uint8_t>{val.value()};
});


// Rejestracja Metod i eventow
com->Add(main_servo_status_event);
com->Add(vent_servo_status_event);
com->Add(set_servo_val);
com->Add(set_vent_val);
com->Add(read_servo_val);
com->Add(read_vent_val);

return core::ErrorCode::kOk;
}

}  // namespace service
}  // namespace simba
