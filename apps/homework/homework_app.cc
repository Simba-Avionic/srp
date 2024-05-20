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
  this->dtc_temp_error = std::make_shared<diag::dtc::DTCObject>(0x22);
  diag_controller.RegisterDTC(dtc_temp_error);
  AppLogger::Info("dtc controller");
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
  AppLogger::Info("diode method done");
  auto temp1 = std::make_shared<com::someip::EventProxyBase>(
      "HomeworkApp/temp1",
      [this](const std::vector<uint8_t> data) {
        AppLogger::Info("Temp1 START");
        float temp;
        std::memcpy(&temp, data.data(), sizeof(temp));
        if (temp > 30) {
          this->dtc_temp_error->Failed();
        } else {
          this->dtc_temp_error->Pass();
        }
        AppLogger::Info("Sensor 1: " + std::to_string(temp));});
  AppLogger::Info("temp 1 done");
  auto temp2 = std::make_shared<com::someip::EventProxyBase>(
      "HomeworkApp/temp2",
      [this](const std::vector<uint8_t> data) {
        AppLogger::Info("Temp2 START");
        float temp;
        std::memcpy(&temp, data.data(), sizeof(temp));
        if (temp > 30) {
          this->dtc_temp_error->Failed();
        } else {
          this->dtc_temp_error->Pass();
        }
        AppLogger::Info("Sensor 2: " + std::to_string(temp));});
  AppLogger::Info("temp 2 done");
  auto temp3 = std::make_shared<com::someip::EventProxyBase>(
      "HomeworkApp/temp3",
      [this](const std::vector<uint8_t> data) {
        AppLogger::Info("Temp3 START");
        float temp;
        std::memcpy(&temp, data.data(), sizeof(temp));
        if (temp > 30) {
          this->dtc_temp_error->Failed();
        } else {
          this->dtc_temp_error->Pass();
        }
        AppLogger::Info("Sensor 3: " + std::to_string(temp));});
  AppLogger::Info("temp 3 done");
  com->Add(temp1);
  com->Add(temp2);
  com->Add(temp3);
  AppLogger::Info("temps com added");
  temp1->StartFindService();
  temp2->StartFindService();
  temp3->StartFindService();
  AppLogger::Info("temp StartFindService done");
  com->Add(diodeMethod);
  AppLogger::Info("diodeMethod com added");
  this->SleepMainThread();
  return core::ErrorCode::kOk;
}
core::ErrorCode Homework::Initialize(
  const std::unordered_map<std::string, std::string>& parms) {
  this->gpio_ = gpio::GPIOController(new com::soc::IpcSocket());
  this->gpio_.Init(666);
  return core::ErrorCode::kOk;
}
}  // namespace simba
