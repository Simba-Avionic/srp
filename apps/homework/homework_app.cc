/**
 * @file homework_app.cc
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <memory>
#include <vector>

#include "apps/homework/homework_app.h"
#include "core/logger/Logger.h"
#include "communication-core/someip-controller/event_proxy.h"
#include "communication-core/someip-controller/method_skeleton.h"

namespace simba {
core::ErrorCode Homework::Run(std::stop_token token) {
    auto diodeMethod = std::make_shared<com::someip::MethodSkeleton>(
      "HomeworkApp/diodeMethod",
      [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
            gpio::Value value;
            if (payload[1] == 0) {
              value = gpio::Value::LOW;
            } else if (payload[1] == 1) {
              value = gpio::Value::HIGH;
            } else {
              return std::vector<uint8_t>{0};
            }
            if (this->gpio_.SetPinValue(payload[0], value) != core::ErrorCode::kOk) {
              return std::vector<uint8_t>{0};
            }
            return std::vector<uint8_t>{1};
      });
    uint8_t sensorAmount = 3;
    std::vector<std::shared_ptr<com::someip::EventProxyBase>> events;
    for (uint8_t i = 1; i <= sensorAmount; i++) {
      events.push_back(std::make_shared<com::someip::EventProxyBase>(
      "HomeworkApp/temp" + std::to_string(i),
      [this, i](const std::vector<uint8_t> data) {
        float temp;
        std::memcpy(&temp, data.data(), sizeof(temp));
        AppLogger::Info("Sensor " + std::to_string(i) + ": " + std::to_string(temp));}));
    }
    for (const auto& event : events) {
      com->Add(event);
      event->StartFindService();
    }
    com->Add(diodeMethod);
    return core::ErrorCode::kOk;
}
core::ErrorCode Homework::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
    this->gpio_ = gpio::GPIOController(new com::soc::IpcSocket());
    this->gpio_.Init(666);
    return core::ErrorCode::kOk;
}
}  // namespace simba
