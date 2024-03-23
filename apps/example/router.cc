/**
 * @file router.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "apps/example/router.h"

#include <memory>
#include <vector>

#include "communication-core/someip-controller/event_proxy.h"
#include "communication-core/someip-controller/method_proxy.h"
#include "communication-core/someip-controller/method_skeleton.h"
#include "core/logger/Logger.h"
#include "diag/dtc/controller/dtc.h"
namespace simba {
namespace router {

#define EEPROM_DEVICE "/dev/i2c-2"  // Ścieżka do urządzenia I2C
#define EEPROM_ADDRESS 0x50         // Adres EEPROM
#define DATA_SIZE 128

core::ErrorCode Router::Run(std::stop_token token) {
  auto example = std::make_shared<com::someip::MethodSkeleton>(
      "ExampleApp/exampleMethod",
      [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
        return std::vector<uint8_t>{0, 1, 2};
      });
  auto example2 = std::make_shared<com::someip::MethodSkeleton>(
      "ExampleApp/exampleMethod2",
      [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
        return std::vector<uint8_t>{0, 1, 2};
      });
  auto servo  = std::make_shared<com::someip::MethodProxyBase>("ExampleApp/setServoValue");
  com->Add(example);
  com->Add(servo);
  com->Add(example2);
  servo->StartFindService();
  auto dtc = std::make_shared<diag::dtc::DTCObject>(20);
  diag_controller.RegisterDTC(dtc);
  while (true) {
    AppLogger::Debug("AppLogger::Debug");
    AppLogger::Info("AppLogger::Info");
    dtc->Pass();
    this->gpio_.SetPinValue(1, gpio::Value::HIGH);
    servo->Get({1});
    std::this_thread::sleep_for(std::chrono::seconds(1));
    this->gpio_.SetPinValue(1, gpio::Value::LOW);
    servo->Get({0});
    dtc->Failed();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode Router::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  this->gpio_ = gpio::GPIOController(new com::soc::IpcSocket());
  this->gpio_.Init(12);
  return core::ErrorCode::kOk;
}
}  // namespace router
}  // namespace simba
