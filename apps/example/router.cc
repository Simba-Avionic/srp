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
  uint8_t servo_pos;
  auto proxy_event = std::make_shared<com::someip::EventProxyBase>(
      "ExampleApp/someevent",
      [this](const std::vector<uint8_t>) { AppLogger::Info("EVENT !!!!!!!"); });
  auto example = std::make_shared<com::someip::MethodSkeleton>(
      "ExampleApp/exampleMethod",
      [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
        return std::vector<uint8_t>{0, 1, 2};
      });
  auto off_prime = std::make_shared<com::someip::MethodProxyBase>("ExampleApp/offPrime");
  auto on_prime = std::make_shared<com::someip::MethodProxyBase>("ExampleApp/onPrime");
  auto servo  = std::make_shared<com::someip::MethodProxyBase>("ExampleApp/setServoValue");
  com->Add(example);
  com->Add(on_prime);
  com->Add(servo);
  com->Add(off_prime);
  com->Add(proxy_event);
  on_prime->StartFindService();
  servo->StartFindService();
  off_prime->StartFindService();
  proxy_event->StartFindService();
  auto dtc = std::make_shared<diag::dtc::DTCObject>(0x20);
  diag_controller.RegisterDTC(dtc);
  while (true) {
    dtc->Pass();
    std::ignore = on_prime->Get();
    servo->Get({1});
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::ignore = off_prime->Get();
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
