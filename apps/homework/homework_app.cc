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

namespace simba {
core::ErrorCode Homework::Run(std::stop_token token) {
    auto temp1 = std::make_shared<com::someip::EventProxyBase>(
      "JakuApp/temp1",
      [this](const std::vector<uint8_t> data) {
        float temp;
        std::memcpy(&temp, data.data(), sizeof(temp)); 
        AppLogger::Info("Sensor 1: " + std::to_string(temp)); });

    auto temp2 = std::make_shared<com::someip::EventProxyBase>(
      "JakuApp/temp2",
      [this](const std::vector<uint8_t> data) {
        float temp;
        std::memcpy(&temp, data.data(), sizeof(temp)); 
        AppLogger::Info("Sensor 2: " + std::to_string(temp)); });

    auto temp3 = std::make_shared<com::someip::EventProxyBase>(
      "JakuApp/temp3",
      [this](const std::vector<uint8_t> data) {
        float temp;
        std::memcpy(&temp, data.data(), sizeof(temp)); 
        AppLogger::Info("Sensor 3: " + std::to_string(temp)); });
    com->Add(temp1);
    com->Add(temp2);
    com->Add(temp3);
    return core::ErrorCode::kOk;
}
core::ErrorCode Homework::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
    this->gpio_ = gpio::GPIOController(new com::soc::IpcSocket());
    this->gpio_.Init(666);
    return core::ErrorCode::kOk;
}
}  // namespace simba
