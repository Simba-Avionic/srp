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

#include <utility>
#include <chrono>  // NOLINT

#include "srp/apps/PrimerService/PrimerServiceHandler.h"
#include "srp/apps/ServoService/ServoServiceHandler.h"
#include "core/common/condition.h"
#include "ara/log/log.h"
#include "core/json/json_parser.h"

namespace srp {
namespace apps {

namespace {
  static constexpr auto kPrimer_path_name =     "srp/apps/EngineService/PrimerService";
  static constexpr auto kServo_path_name =      "srp/apps/EngineService/ServoService";
  static constexpr auto kMain_path_name =       "srp/apps/EngineService/MainService";
  static constexpr auto kEngine_path_name =     "srp/apps/EngineService/EngineService_ipc";
  static constexpr auto kEngine_udp_path_name = "srp/apps/EngineService/EngineService_udp";
  static constexpr auto kLogger_path_name =     "srp/apps/EngineService/FileLoggerApp";
  static constexpr auto kInit_max_intervals =   20;
  static constexpr auto kPrimerDelay =          1000;
  static constexpr auto kPin_off =              0;
  static constexpr auto kPin_on =               1;
  static constexpr auto kHeartBeatPinID =       1;
  using RocketState_t = core::rocketState::RocketState_t;
}  // namespace

std::optional<std::vector<ArmPinConfig_t>> EngineApp::LoadArmPinConfig(const std::string& path) {
    auto parser = core::json::JsonParser::Parser(path);
    if (!parser.has_value()) {
      ara::log::LogError() << "LoadArmPinConfig: Failed to parse config file at " << path;
      return std::nullopt;
    }
    ara::log::LogDebug() << "LoadArmPinConfig: Config file parsed successfully";
    auto pins = parser.value().GetArray<nlohmann::json>("arm_pins");
    if (!pins.has_value()) {
      ara::log::LogError() << "LoadArmPinConfig: 'arm_pins' array not found in config";
      return std::nullopt;
    }
    std::vector<ArmPinConfig_t> respons;
    for (const auto& entry : pins.value()) {
      auto pin_config = core::json::JsonParser::Parser(entry);
      if (!pin_config.has_value()) {
        continue;
      }
      auto arm_pin_id = pin_config.value().GetNumber<uint8_t>("id");
      auto arm_pin_desc = pin_config.value().GetString("desc");
      auto arm_pin_func = pin_config.value().GetString("func");
      if (!arm_pin_func.has_value() || !arm_pin_id.has_value()) {
        continue;
      }
      respons.push_back(ArmPinConfig_t{arm_pin_id.value(), arm_pin_desc.value_or(""), arm_pin_func.value()});
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
      main_proxy(ara::core::InstanceSpecifier{kMain_path_name}),
      logger_proxy{ara::core::InstanceSpecifier{kLogger_path_name}},
      primer_handler_{nullptr}, servo_handler_{nullptr}, main_handler{nullptr}, logger_handler_{nullptr} {
  ara::log::LogDebug() << "EngineApp constructor: Initializing EngineApp instance";
}

int EngineApp::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "EngineApp::Run: Starting main event loop";
  while (!token.stop_requested()) {
    ara::log::LogDebug() << "EngineApp::Run: Main loop iteration";

    if (gpio_.SetPinValue(kHeartBeatPinID, kPin_on, 500) != core::ErrorCode::kOk) {
      ara::log::LogWarn() << "EngineApp::Run: Failed to toggle heartbeat pin";
    }

    auto state = state_ctr->GetState();
    ara::log::LogDebug() << "EngineApp::Run: Current state = " << std::to_string(static_cast<int>(state));

    service_ipc.CurrentMode.Update(static_cast<uint8_t>(state));
    service_udp.CurrentMode.Update(static_cast<uint8_t>(state));
    ara::log::LogDebug() << "EngineApp::Run: Updated service state";

    core::condition::wait_for(std::chrono::seconds(1), token);
  }

  ara::log::LogInfo() << "EngineApp::Run: Stop requested, shutting down services";
  service_ipc.StopOffer();
  service_udp.StopOffer();

  servo_proxy.StopFindService();
  primer_proxy.StopFindService();
  main_proxy.StopFindService();
  ara::log::LogInfo() << "EngineApp::Run: Shutdown complete";
  return 0;
}

int EngineApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
  ara::log::LogDebug() << "EngineApp::Initialize: Waiting 5 seconds for system stabilization";
  if (parms.find("app_path") == parms.end()) {
      return 1;
  }
  std::this_thread::sleep_for(std::chrono::seconds(5));
  ara::log::LogInfo() << "EngineApp::Initialize: Starting initialization sequence";
  std::string config_path = std::string(parms.at("app_path")) + "etc/config.json";
  ara::log::LogDebug() << "EngineApp::Initialize: Loading arm pins from " << config_path;
  auto arm_pins = LoadArmPinConfig(config_path);
  if (!arm_pins.has_value()) {
    ara::log::LogError() << "EngineApp::Initialize: Failed to load arm pins configuration";
    return 1;
  }
  this->arm_pins_id = std::move(arm_pins.value());
  state_ctr = core::rocketState::RocketStateController::GetInstance();
  state_ctr->RegisterRequirementsCallback([this](RocketState_t state) {
    switch (state) {
      case RocketState_t::ARM:
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
  state_ctr->RegisterOnStateChangeCallback([this](RocketState_t state) {
        this->OnStateChange(state);
  });
  state_ctr->RegisterCallback(RocketState_t::CONNECTION_LOST, [this]() {
    ara::log::LogDebug() << "EngineApp::RegisterCallback: CONNECTION_LOST callback triggered";
    this->OnDisarm();
  });

  state_ctr->RegisterCallback(RocketState_t::LAUNCH, [this]() {
    ara::log::LogDebug() << "EngineApp::RegisterCallback: LAUNCH callback triggered";
    this->OnLaunch();
  });
  state_ctr->RegisterCallback(RocketState_t::ARM, [this]() {
    ara::log::LogDebug() << "EngineApp::RegisterCallback: ARM callback triggered";
    this->OnArm();
  });
  state_ctr->RegisterCallback(RocketState_t::DISARM, [this]() {
    ara::log::LogDebug() << "EngineApp::RegisterCallback: DISARM callback triggered";
    this->OnDisarm();
  });
  state_ctr->RegisterCallback(RocketState_t::APOGEE, [this]() {
    ara::log::LogDebug() << "EngineApp::RegisterCallback: APOGEE callback triggered";
    this->OnApogee();
  });
  state_ctr->RegisterCallback(RocketState_t::ABORT, [this]() {
    ara::log::LogDebug() << "EngineApp::RegisterCallback: ABORT callback triggered";
    this->OnAbort();
  });
  ara::log::LogDebug() << "EngineApp::Initialize: All state callbacks registered";

  ara::log::LogDebug() << "EngineApp::Initialize: Starting service discovery";
  servo_proxy.StartFindService([this](auto handler) {
    ara::log::LogDebug() << "EngineApp::Initialize: Servo service handler found";
    servo_handler_ = handler;
  });
  primer_proxy.StartFindService([this](auto handler) {
    ara::log::LogDebug() << "EngineApp::Initialize: Primer service handler found";
    this->primer_handler_ = handler;
  });
  logger_proxy.StartFindService([this](auto handler) {
    ara::log::LogDebug() << "EngineApp::Initialize: Logger service handler found";
    this->logger_handler_ = handler;
  });

  main_proxy.StartFindService([this](auto handler) {
    ara::log::LogDebug() << "EngineApp::Initialize: Main service handler found";
    main_handler = handler;
  });

  service_ipc.StartOffer();
  service_udp.StartOffer();

  int i = 0;
  while ((servo_handler_ == nullptr || primer_handler_ == nullptr) && i < kInit_max_intervals) {
    i += 1;
    ara::log::LogWarn() << "Retry setting servo handler or primer_handler i: " << std::to_string(i-1);
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
  state_ctr->SetState(RocketState_t::DISARM);
  ara::log::LogDebug() << "End Initialization Engine App";
  return 0;
}

void EngineApp::OnStateChange(RocketState_t new_state) {
  ara::log::LogDebug() << "EngineApp::OnStateChange: Updating services with new state "
                                        << std::to_string(static_cast<int>(new_state));
  service_ipc.CurrentMode.Update(static_cast<uint8_t>(new_state));
  service_udp.CurrentMode.Update(static_cast<uint8_t>(new_state));
}


void EngineApp::OnLaunch() {
  ara::log::LogInfo() << "EngineApp::OnLaunch: Launch initiated - starting async launch sequence";
  std::thread([this]() {
    auto res = this->primer_handler_->StartPrime();
    if (!res.HasValue()) {
      ara::log::LogError() << "Invalid request to MW:GPIOService";
      return;
    }
    ara::log::LogDebug() << "EngineApp::OnLaunch: Primer started successfully";

    ara::log::LogDebug() << "EngineApp::OnLaunch: Waiting "
                          << std::to_string(kPrimerDelay) << "ms before servo activation";
    std::this_thread::sleep_for(std::chrono::milliseconds(kPrimerDelay));
    auto res2 = this->servo_handler_->SetMainServoValue(1);
    if (!res2.HasValue()) {
      ara::log::LogError() << "Invalid request to MW:I2CService";
      return;
    }
    ara::log::LogDebug() << "EngineApp::OnLaunch: Main servo activated successfully";

    if (main_handler) {
      main_handler->setMode(static_cast<uint8_t>(RocketState_t::FLIGHT));
    }
    state_ctr->SetState(RocketState_t::FLIGHT);
    ara::log::LogDebug() << "EngineApp::OnLaunch: Async launch sequence complete";
  }).detach();
}

void EngineApp::OnArm() {
  if (logger_handler_) {
    this->logger_handler_->Start();
  }
  for (const auto & pin : arm_pins_id) {
    if (gpio_.SetPinValue(pin.pin_id, kPin_on) != core::ErrorCode::kOk) {
      ara::log::LogError() << "cant arm pin: " << pin.name;
    }
  }
}

void EngineApp::OnDisarm() {
  ara::log::LogInfo() << "EngineApp::OnDisarm: DISARM sequence completed";
  for (const ArmPinConfig_t& pin : arm_pins_id) {
    bool disable_later = (pin.func == "VS" || pin.func == "PFMS");
    if (gpio_.SetPinValue(pin.pin_id,
                          disable_later ? kPin_on : kPin_off,
                          disable_later ? 3500 : 0,
                          disable_later) != core::ErrorCode::kOk) {
      ara::log::LogError() << "cant disarm pin: " << pin.name;
    }
  }
  if (servo_handler_ != nullptr) {
    servo_handler_->SetDumpValue(0);
    servo_handler_->SetVentServoValue(0);
  }
  if (logger_handler_) {
    this->logger_handler_->Stop();
  }
}

void EngineApp::OnApogee() {
  ara::log::LogInfo() << "EngineApp::OnApogee: Apogee detected - triggering abort sequence";
  OnAbort();
}

void EngineApp::OnAbort() {
  ara::log::LogWarn() << "EngineApp::OnAbort: ABORT sequence initiated";

  for (const ArmPinConfig_t& pin : arm_pins_id) {
    bool disable_later = (pin.func == "VS" || pin.func == "PFMS");
    if (gpio_.SetPinValue(pin.pin_id,
                          disable_later ? kPin_on : kPin_off,
                          disable_later ? 3500 : 0,
                          disable_later) != core::ErrorCode::kOk) {
      ara::log::LogError() << "cant disarm pin: " << pin.name;
    }
  }
  if (servo_handler_ != nullptr) {
    servo_handler_->SetDumpValue(1);
    servo_handler_->SetVentServoValue(2);
  }
}

}  // namespace apps
}  // namespace srp
