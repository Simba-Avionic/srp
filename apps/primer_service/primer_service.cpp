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
#include <future>  // NOLINT

#include "apps/primer_service/primer_service.hpp"
#include "communication-core/someip-controller/event_proxy.h"
#include "communication-core/someip-controller/method_skeleton.h"
#include "communication-core/someip-controller/event_skeleton.h"
#include "diag/dtc/controller/dtc.h"
#include "nlohmann/json.hpp"
#include "core/json/json_parser.h"

namespace simba {
namespace primer {

namespace {
  const constexpr uint8_t IGNITER_PIN = 0x0D;
  const constexpr uint16_t IGNITER_ACTIVE_TIME = 500;
}


core::ErrorCode PrimerService::Run(std::stop_token token) {
  auto dtc_30 = std::make_shared<diag::dtc::DTCObject>(0x30);
  auto primerOnMethod = std::make_shared<com::someip::MethodSkeleton>(
      "PrimerApp/onPrime",
      [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
            AppLogger::Debug("Receive onPrime method");
            if (this->primerState == gpio::Value::LOW) {
            this->gpio_.SetPinValue(IGNITER_PIN, gpio::Value::HIGH);
            this->primerState = gpio::Value::HIGH;
            return std::vector<uint8_t>{1};
            }
            return std::vector<uint8_t>{0};
  });
    auto primerOffMethod = std::make_shared<com::someip::MethodSkeleton>(
      "PrimerApp/offPrime",
      [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
            AppLogger::Debug("Receive offPrime method");
            if (this->primerState == gpio::Value::HIGH) {
            this->gpio_.SetPinValue(IGNITER_PIN, gpio::Value::LOW);
            this->primerState = gpio::Value::LOW;
            return std::vector<uint8_t>{1};
            }
            return std::vector<uint8_t>{0};
  });
  auto startPrimeMethod = std::make_shared<com::someip::MethodSkeleton>(
      "PrimerApp/startPrime",
      [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
            auto future = std::async(std::launch::async, [this](){
            this->gpio_.SetPinValue(IGNITER_PIN, gpio::Value::HIGH);
            std::this_thread::sleep_for(std::chrono::milliseconds(IGNITER_ACTIVE_TIME));
            this->gpio_.SetPinValue(IGNITER_PIN, gpio::Value::LOW);
            });
            return std::vector<uint8_t>{0x1};
  });
  auto event_example =
      std::make_shared<com::someip::EventSkeleton>("PrimerApp/primeStatusEvent");

  com->Add(primerOnMethod);
  com->Add(primerOffMethod);
  com->Add(startPrimeMethod);
  while (true) {
    event_example->SetValue({static_cast<uint8_t>(this->primerState)});
    AppLogger::Info("send prim Value"+std::to_string(this->primerState));
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return core::ErrorCode::kOk;
}
uint8_t ReadConfig(const std::unordered_map<std::string, std::string>& parms) {
  std::ifstream file{"/opt/" + parms.at("app_name") + "/etc/config.json"};
  if (!file.is_open()) {
    AppLogger::Warning("cant find config file, use DEFAULT IGNITER ID");
    return IGNITER_PIN;
  }
  nlohmann::json data = nlohmann::json::parse(file);
  if (!data.contains("primer")) {
    AppLogger::Warning("invalid config format, use DEFAULT IGNITER ID");
    return IGNITER_PIN;
  }
  return static_cast<uint8_t>(data.at("primer"));
}

core::ErrorCode PrimerService::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  this->gpio_ = gpio::GPIOController(new com::soc::IpcSocket());
  this->gpio_.Init(this->app_id_);

  return core::ErrorCode::kOk;
}

}  // namespace primer
}  // namespace simba
