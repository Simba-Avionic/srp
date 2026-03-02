/**
 * @file engine_app.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "apps/ec/engine_service/engine_app.hpp"
#include "srp/apps/PrimerService/PrimerServiceHandler.h"
#include "srp/apps/ServoService/ServoServiceHandler.h"
#include "core/common/condition.h"
#include "ara/log/log.h"
#include "core/json/json_parser.h"

namespace srp {
namespace apps {

namespace {
  static constexpr auto kPrimer_path_name = "srp/apps/EngineService/PrimerService";
  static constexpr auto kServo_path_name = "srp/apps/EngineService/ServoService";
  static constexpr auto kEngine_path_name = "srp/apps/EngineService/EngineService_ipc";
  static constexpr auto kEngine_udp_path_name = "srp/apps/EngineService/EngineService_udp";
  static constexpr auto kInit_max_intervals = 20;
  static constexpr auto kPrimerDelay = 1000;
  static constexpr auto kPin_off = 0;
  static constexpr auto kPin_on = 1;
}

std::optional<std::vector<ArmPinConfig_t>> EngineApp::LoadArmPinConfig(const std::string& path) {
    auto parser = core::json::JsonParser::Parser(path);
    if (!parser.has_value()) {
      return std::nullopt;
    }
    auto pins = parser.value().GetArray<nlohmann::json>("arm_pins");
    if (!pins.has_value()) {
      return std::nullopt;
    }
    std::vector<ArmPinConfig_t> respons;
    for (auto& entry : pins.value()) {
      auto pin_config = core::json::JsonParser::Parser(entry);
      if (!pin_config.has_value()) {
        continue;
      }
      auto arm_pin_id = pin_config.value().GetNumber<uint8_t>("id");
      auto arm_pin_desc = pin_config.value().GetString("desc");
      if (arm_pin_desc.has_value() || arm_pin_id.has_value()) {
        continue;
      }
      respons.push_back(ArmPinConfig_t{arm_pin_id.value(), arm_pin_desc.value());
    }
    if (respons.size() == 0) {
      return std::nullopt;
    }
    return respons;

}

EngineApp::EngineApp():
      primer_proxy(ara::core::InstanceSpecifier{kPrimer_path_name}),
      servo_proxy((ara::core::InstanceSpecifier{kServo_path_name})),
      service_ipc(ara::core::InstanceSpecifier{kEngine_path_name}),
      service_udp(ara::core::InstanceSpecifier{kEngine_udp_path_name}),
      primer_handler_{nullptr}, servo_handler_{nullptr} {
}

int EngineApp::Run(const std::stop_token& token) {
  core::condition::wait(token);
  service_ipc.StopOffer();
  service_udp.StopOffer();
  servo_proxy.StopFindService();
  primer_proxy.StopFindService();
  return 0;
}

int EngineApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
  if (parms.find("app_path") == parms.end()) {
      return 1;
  }
  std::string path(parms.at("app_path"));
  path += "etc/config.json";
  auto arm_pins = LoadArmPinConfig(path);
  if (!arm_pins.has_value()) {
    ara::log::LogError() << "cant load arm pins";
    return 1;
  }
  this->arm_pins_id = std::move(arm_pins.value());
    
  state_ctr = core::rocketState::RocketStateController::GetInstance();
  state_ctr->RegisterRequirementsCallback([this](core::rocketState::RocketState_t state) {
    switch (state) {
      case core::rocketState::RocketState_t::LAUNCH:
        break;
      case core::rocketState::RocketState_t::ARM:
        if ((primer_handler_ == nullptr || servo_handler_ == nullptr)) {
          ara::log::LogError() << "Invalid pointer to Primer or Servo";
          return false;
        }
        break;
      default:
        break;
    }
    return true;
  });
  state_ctr->RegisterOnStateChangeCallback([this](core::rocketState::RocketState_t state) {
        this->OnStateChange(state);
  });
  state_ctr->RegisterCallback(core::rocketState::RocketState_t::LAUNCH, [this]() {
      this->OnLaunch();
  });
  state_ctr->RegisterCallback(core::rocketState::RocketState_t::ARM, [this]() {
      this->OnArm();
  });
  state_ctr->RegisterCallback(core::rocketState::RocketState_t::DISARM, [this]() {
      this->OnDisarm();
  });
  state_ctr->RegisterCallback(core::rocketState::RocketState_t::APOGEE, [this]() {
      this->OnApogee();
  });
  state_ctr->RegisterCallback(core::rocketState::RocketState_t::ABORT, [this]() {
      this->OnAbort();
  });

  servo_proxy.StartFindService([this](auto handler) {
    servo_handler_ = handler;
  });
  primer_proxy.StartFindService([this](auto handler) {
    this->primer_handler_ = handler;
  });

  service_ipc.StartOffer();
  service_udp.StartOffer();

  int i = 0;
  while ((servo_handler_ == nullptr || primer_handler_ == nullptr) && i < kInit_max_intervals) {
    i += 1;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  if (i >= kInit_max_intervals) {
    ara::log::LogFatal() << "Cant initialize servo handler or primer handler, closing app";
    return 1;
  }

  if (state_ctr == nullptr) {
    ara::log::LogError() << "EngineApp::Initialize: state_ctr_ is nullptr!";
    return 1;
  }
  ara::log::LogInfo() << "Initialize Complete";
  state_ctr->SetState(core::rocketState::RocketState_t::DISARM);
  return 0;
}

void EngineApp::OnStateChange(core::rocketState::RocketState_t new_state) {
  service_ipc.CurrentMode.Update(static_cast<uint8_t>(new_state));
  service_udp.CurrentMode.Update(static_cast<uint8_t>(new_state));
}


void EngineApp::OnLaunch() {
  auto res = this->primer_handler_->OnPrime();
  if (!res.HasValue()) {
    ara::log::LogError() << "Invalid request to MW:GPIOService";
    return;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(kPrimerDelay));
  auto res2 = this->servo_handler_->SetMainServoValue(1);
  this->primer_handler_->OffPrime();
  if (!res2.HasValue()) {
    ara::log::LogError() << "Invalid request to MW:I2CService";
    return;
  }
  state_ctr->SetState(core::rocketState::RocketState_t::FLIGHT);
}

void EngineApp::OnArm() {
  for (const auto & pin : arm_pins_id) {
    if (gpio_.SetPinValue(pin.pin_id, kPin_on) != core::ErrorCode::kOk) {
      ara::log::LogError() << "cant arm pin: " << pin.name;
    }
  }
}
void EngineApp::OnDisarm() {
  for (const auto & pin : arm_pins_id) {
    if (gpio_.SetPinValue(pin.pin_id, kPin_off) != core::ErrorCode::kOk) {
      ara::log::LogError() << "cant disarm pin: " << pin.name;
    }
  }
}
void EngineApp::OnApogee() {

}
void EngineApp::OnAbort() {
  for (const ArmPinConfig_t& pin: arm_pins_id) {
    if (pin.name == "Vent Servo Power" || pin.name == "Dump Valve Servo Power") {
      if (gpio_.SetPinValue(pin.pin_id, kPin_on) != core::ErrorCode::kOk) {
        ara::log::LogError() << "cant arm pin: " << pin.name;
      }
    }
  }
  servo_handler_->SetDumpValue(1);
  servo_handler_->SetVentServoValue(1);
  std::this_thread::sleep_for(std::chrono::seconds(2));
  for (const ArmPinConfig_t& pin: arm_pins_id) {
    if (pin.name == "Vent Servo Power" || pin.name == "Dump Valve Servo Power") {
      if (gpio_.SetPinValue(pin.pin_id, kPin_off) != core::ErrorCode::kOk) {
        ara::log::LogError() << "cant disarm pin: " << pin.name;
      }
    }
  }
}

}  // namespace apps
}  // namespace srp
