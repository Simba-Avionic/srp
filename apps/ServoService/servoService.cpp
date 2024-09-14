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
#include "servoService.hpp"

#include <memory>
#include <optional>
#include <vector>

#include "ara/log/log.h"
#include "core/common/condition.h"
namespace simba {
namespace service {

int ServoService::Run(const std::stop_token& token) {
  servo_service_did_->StartOffer();
  while (!token.stop_requested()) {
    // update servo positions;
    auto val = this->servo_controller.ReadServoPosition(60);
    if (val.has_value()) {
      // main_servo_status_event->SetValue({val.value()});
    }
    auto val2 = this->servo_controller.ReadServoPosition(61);
    if (val2.has_value()) {
      // vent_servo_status_event->SetValue({val2.value()});
    }
    ara::log::LogInfo() << ("Send servo status event");
    core::condition::wait_for(std::chrono::milliseconds(975),token);
  }
  servo_service_did_->StopOffer();
  return 0;
}

int ServoService::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  //  this->servo_controller.Init(parms,
  //  std::make_unique<i2c::I2CController>());
  this->servo_controller.Init(parms.at("app_path"),
                              std::make_unique<simba::i2c::I2CController>(),
                              std::make_unique<gpio::GPIOController>());
  servo_service_did_ = std::make_unique<ServoServiceDiD>(ara::core::InstanceSpecifier("/simba/apps/servoService/ServoStatus"));
  // main_servo_status_event =
  //     std::make_shared<com::someip::EventSkeleton>("ServoApp/servoStatusEvent");
  // vent_servo_status_event = std::make_shared<com::someip::EventSkeleton>(
  //     "ServoApp/servoVentStatusEvent");
  // this->set_servo_val = std::make_shared<com::someip::MethodSkeleton>(
  //     "ServoApp/setServoValue",
  //     [this](const std::vector<uint8_t> payload)
  //         -> std::optional<std::vector<uint8_t>> {
  //       if (payload.size() == 1) {
  //         AppLogger::Debug("move servo id: 60 to pos" +
  //                          std::to_string(static_cast<int>(payload[0])));
  //         auto res =
  //             this->servo_controller.AutoSetServoPosition(60, payload[0]);
  //         if (res != core::ErrorCode::kOk) {
  //           return std::vector<uint8_t>{0};
  //         }
  //         return std::vector<uint8_t>{1};
  //       }
  //       return {};
  //     });
  // this->set_vent_val = std::make_shared<com::someip::MethodSkeleton>(
  //     "ServoApp/setServoVentValue",
  //     [this](const std::vector<uint8_t> payload)
  //         -> std::optional<std::vector<uint8_t>> {
  //       if (payload.size() == 1) {
  //         AppLogger::Debug("move servo id: 61 to pos" +
  //                          std::to_string(static_cast<int>(payload[0])));
  //         auto res =
  //             this->servo_controller.AutoSetServoPosition(61, payload[0]);
  //         if (res != core::ErrorCode::kOk) {
  //           return std::vector<uint8_t>{0};
  //         }
  //         return std::vector<uint8_t>{1};
  //       }
  //       return {};
  //     });

  // auto read_servo_val = std::make_shared<com::someip::MethodSkeleton>(
  //     "ServoApp/readServoValue",
  //     [this](const std::vector<uint8_t> payload)
  //         -> std::optional<std::vector<uint8_t>> {
  //       auto val = this->servo_controller.ReadServoPosition(60);
  //       if (!val.has_value()) {
  //         return {};
  //       }
  //       return std::vector<uint8_t>{val.value()};
  //     });
  // auto read_vent_val = std::make_shared<com::someip::MethodSkeleton>(
  //     "ServoApp/readServoVentValue",
  //     [this](const std::vector<uint8_t> payload)
  //         -> std::optional<std::vector<uint8_t>> {
  //       auto val = this->servo_controller.ReadServoPosition(61);
  //       if (!val.has_value()) {
  //         return {};
  //       }
  //       return std::vector<uint8_t>{val.value()};
  //     });

  // // Rejestracja Metod i eventow
  // com->Add(main_servo_status_event);
  // com->Add(vent_servo_status_event);
  // com->Add(set_servo_val);
  // com->Add(set_vent_val);
  // com->Add(read_servo_val);
  // com->Add(read_vent_val);

  return 0;
}

}  // namespace service
}  // namespace simba
