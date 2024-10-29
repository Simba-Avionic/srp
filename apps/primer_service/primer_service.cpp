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
#include "core/common/condition.h"
#include "core/json/json_parser.h"
#include "ara/log/log.h"

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
  // if (this->primerState != state) {
  //   core::ErrorCode error;
  //   for (const auto primer : primer_pins_) {
  //     error = this->gpio_.SetPinValue(primer, state);
  //     if (error != core::ErrorCode::kOk) {
  //       this->dtc_31->Failed();
  //       return core::ErrorCode::kError;
  //     }
  //   }
  //   this->primerState = state;
  //   this->primer_event->SetValue({static_cast<uint8_t>(state)});
  //   return core::ErrorCode::kOk;
  // }
  return core::ErrorCode::kNotDefine;
}


int PrimerService::Run(const std::stop_token& token) {
  // auto primerOnMethod = std::make_shared<com::someip::MethodSkeleton>(
  //     "PrimerApp/onPrime",
  //     [this](const std::vector<uint8_t> payload)
  //         -> std::optional<std::vector<uint8_t>> {
  //           AppLogger::Debug("Receive onPrime method");
  //           if (this->ChangePrimerState(ON_INGITER) == core::ErrorCode::kOk) {
  //             return std::vector<uint8_t>{1};
  //           }
  //           return std::vector<uint8_t>{0};
  // });
  // auto primerOffMethod = std::make_shared<com::someip::MethodSkeleton>(
  //     "PrimerApp/offPrime",
  //     [this](const std::vector<uint8_t> payload)
  //         -> std::optional<std::vector<uint8_t>> {
  //           AppLogger::Debug("Receive offPrime method");
  //           if (this->ChangePrimerState(OFF_INGITER) == core::ErrorCode::kOk) {
  //             return std::vector<uint8_t>{1};
  //           }
  //           return std::vector<uint8_t>{0};
  // });
  // auto startPrimeMethod = std::make_shared<com::someip::MethodSkeleton>(
  //     "PrimerApp/startPrime",
  //     [this](const std::vector<uint8_t> payload)
  //         -> std::optional<std::vector<uint8_t>> {
  //           auto future = std::async(std::launch::async, [this](){
  //           if (this->ChangePrimerState(ON_INGITER) != core::ErrorCode::kOk) {
  //             return std::vector<uint8_t>{0};
  //           }
  //           std::this_thread::sleep_for(std::chrono::milliseconds(this->active_time));
  //           if (this->ChangePrimerState(OFF_INGITER) != core::ErrorCode::kOk) {
  //             return std::vector<uint8_t>{0};
  //           }
  //           return std::vector<uint8_t>{1};
  //         });
  //         return {};
  // });
  // com->Add(primerOffMethod);
  // com->Add(primerOnMethod);
  // com->Add(startPrimeMethod);
  // this->primer_event->SetValue({static_cast<uint8_t>(this->primerState)});
  core::condition::wait(token);
  return core::ErrorCode::kOk;
}

core::ErrorCode PrimerService::ReadConfig(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
  std::string path =  parms.at("app_path") + "etc/config.json";
  auto parser_opt = core::json::JsonParser::Parser(path);
  this->active_time = IGNITER_ACTIVE_TIME;

  if (!parser_opt.has_value()) {
    ara::log::LogWarn() << 
    ara::log::LogWarn() << "cant find config file, use DEFAULT IGNITER_ID and ACTIVE_TIME";
    this->primer_pins_.push_back(IGNITER_PIN_ID);
    return core::ErrorCode::kInitializeError;
  }
  auto pin_ids_opt = parser_opt.value().GetArray<uint8_t>("primer_ids");
  if (!pin_ids_opt.has_value()) {
    ara::log::LogWarn() << "config dont contains primer field";
  } else {
  this->primer_pins_ = pin_ids_opt.value();
  }
  auto active_time_opt = parser_opt.value().GetNumber<uint16_t>("primer_active_time");
  if (active_time_opt.has_value()) {
    this->active_time = active_time_opt.value();
  }
  return core::ErrorCode::kOk;
}

int PrimerService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
  /**
   * @brief define and register errors
   * 
   */

  this->gpio_ = gpio::GPIOController(std::make_unique<com::soc::StreamIpcSocket>());
  ReadConfig(parms);
  this->primerState = 0;
  return core::ErrorCode::kOk;
}

}  // namespace primer
}  // namespace simba
