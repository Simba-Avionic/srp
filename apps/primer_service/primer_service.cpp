/**
 * @file primer_service.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <memory>
#include <vector>
#include <utility>
#include <future>  // NOLINT

#include "apps/primer_service/primer_service.hpp"
#include "communication-core/someip-controller/event_proxy.h"
#include "communication-core/someip-controller/method_skeleton.h"
#include "nlohmann/json.hpp"
#include "core/json/json_parser.h"

namespace simba {
namespace primer {

namespace {
  /**
   * @brief domyślne wartości
   * 
   */
  const constexpr uint8_t IGNITER_PIN_ID = 5;
  const constexpr uint16_t IGNITER_ACTIVE_TIME = 250;

  const constexpr uint8_t ON_INGITER = 1;
  const constexpr uint8_t OFF_INGITER = 0;
}  // namespace


core::ErrorCode PrimerService::ChangePrimerState(uint8_t state) {
  if (this->primerState != state) {
    core::ErrorCode error;
    for (const auto primer : primer_pins_) {
      error = this->gpio_.SetPinValue(primer, state);
      if (error != core::ErrorCode::kOk) {
        this->dtc_31->Failed();
        return core::ErrorCode::kError;
      }
    }
    this->primerState = state;
    this->primer_event->SetValue({static_cast<uint8_t>(state)});
    return core::ErrorCode::kOk;
  }
  return core::ErrorCode::kNotDefine;
}


core::ErrorCode PrimerService::Run(std::stop_token token) {
  auto primerOnMethod = std::make_shared<com::someip::MethodSkeleton>(
      "PrimerApp/onPrime",
      [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
            AppLogger::Debug("Receive onPrime method");
            if (this->ChangePrimerState(ON_INGITER) == core::ErrorCode::kOk) {
              return std::vector<uint8_t>{1};
            }
            return std::vector<uint8_t>{0};
  });
  auto primerOffMethod = std::make_shared<com::someip::MethodSkeleton>(
      "PrimerApp/offPrime",
      [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
            AppLogger::Debug("Receive offPrime method");
            if (this->ChangePrimerState(OFF_INGITER) == core::ErrorCode::kOk) {
              return std::vector<uint8_t>{1};
            }
            return std::vector<uint8_t>{0};
  });
  auto startPrimeMethod = std::make_shared<com::someip::MethodSkeleton>(
      "PrimerApp/startPrime",
      [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
            auto future = std::async(std::launch::async, [this](){
            if (this->ChangePrimerState(ON_INGITER) != core::ErrorCode::kOk) {
              return std::vector<uint8_t>{0};
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(this->active_time));
            if (this->ChangePrimerState(OFF_INGITER) != core::ErrorCode::kOk) {
              return std::vector<uint8_t>{0};
            }
            return std::vector<uint8_t>{1};
          });
          return {};
  });
  com->Add(primerOffMethod);
  com->Add(primerOnMethod);
  com->Add(startPrimeMethod);
  this->primer_event->SetValue({static_cast<uint8_t>(this->primerState)});
  while (true) {
    this->SleepMainThread();
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode PrimerService::ReadConfig(const std::unordered_map<std::string, std::string>& parms) {
  auto parser_opt = core::json::JsonParser::Parser("/opt/" + parms.at("app_name") + "/etc/config.json");
  this->active_time = IGNITER_ACTIVE_TIME;

  if (!parser_opt.has_value()) {
    AppLogger::Warning("cant find config file, use DEFAULT IGNITER_ID and ACTIVE_TIME");
    this->primer_pins_.push_back(IGNITER_PIN_ID);
    return core::ErrorCode::kInitializeError;
  }
  auto pin_ids_opt = parser_opt.value().GetArray<uint8_t>("primer_ids");
  if (!pin_ids_opt.has_value()) {
    AppLogger::Warning("config dont contains primer field");
  } else {
  this->primer_pins_ = pin_ids_opt.value();
  }
  auto active_time_opt = parser_opt.value().GetNumber<uint16_t>("primer_active_time");
  if (active_time_opt.has_value()) {
    this->active_time = active_time_opt.value();
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode PrimerService::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  /**
   * @brief define and register errors
   * 
   */
  this->dtc_30 = std::make_shared<diag::dtc::DTCObject>(0x30);
  this->dtc_31 = std::make_shared<diag::dtc::DTCObject>(0x31);
  this->diag_controller.RegisterDTC(dtc_30);
  this->diag_controller.RegisterDTC(dtc_31);

  this->gpio_ = gpio::GPIOController(std::make_unique<com::soc::StreamIpcSocket>());
  ReadConfig(parms);
  this->primerState = 0;
  /**
   * @brief register events
   * 
   */
  this->primer_event =
      std::make_shared<com::someip::EventSkeleton>("PrimerApp/primeStatusEvent");
  /**
   * @brief register SOME/IP events in network
   * 
   */
  com->Add(primer_event);
  return core::ErrorCode::kOk;
}

}  // namespace primer
}  // namespace simba
