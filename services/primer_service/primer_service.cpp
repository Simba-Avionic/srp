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

#include "services/primer_service/primer_service.hpp"
#include "communication-core/someip-controller/event_proxy.h"
#include "communication-core/someip-controller/method_skeleton.h"
#include "communication-core/someip-controller/event_skeleton.h"
#include "diag/dtc/controller/dtc.h"

namespace simba {
namespace primer {

namespace {
  const constexpr uint8_t IGNITER_PIN = 0x0D;
}


core::ErrorCode PrimerService::Run(std::stop_token token) {
  auto dtc_30 = std::make_shared<diag::dtc::DTCObject>(0x30);
  auto dtc_B0 = std::make_shared<diag::dtc::DTCObject>(0xB0);
  auto primerOnMethod = std::make_shared<com::someip::MethodSkeleton>(
      "PrimerApp/onPrime",
      [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
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
            this->gpio_.SetPinValue(IGNITER_PIN, gpio::Value::HIGH);
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
            this->gpio_.SetPinValue(IGNITER_PIN, gpio::Value::LOW);
  });
  auto event_example =
      std::make_shared<com::someip::EventSkeleton>("PrimerApp/primeStatusEvent");

  com->Add(primerOnMethod);
  com->Add(primerOffMethod);
  while (true) {
    event_example->SetValue({static_cast<uint8_t>(this->primerState)});
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode PrimerService::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  this->gpio_ = gpio::GPIOController(new com::soc::IpcSocket());
  this->gpio_.Init(this->app_id_);
  return core::ErrorCode::kOk;
}

}  // namespace primer
}  // namespace simba
