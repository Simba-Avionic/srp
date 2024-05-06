/***
 * @file exampleApp.cc
 * @author Jacek Kukawski (jacekka6@gmail.com)
 * @brief This is an example of how to use the Logger class.
 * @version 0.1
 * @date 2024-04-20
 * @copyright Copyright (c) 2024
*/
#include "apps/exampleApp/exampleApp.h"
#include <chrono>
#include <thread>

namespace simba {
namespace example {
core::ErrorCode exampleApp::Run(std::stop_token token) {
  AppLogger::Info("exampleApp::Run() called");
  u_int8_t L0 = 1;
  u_int8_t L3 = 4;
  u_int counter = 0;
  while(!token.stop_requested()){
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if(this->gpio_controller_.GetPinValue(L0) == gpio::Value::HIGH)
      this->gpio_controller_.SetPinValue(L0, gpio::Value::LOW);
    else
      this->gpio_controller_.SetPinValue(L0, gpio::Value::HIGH);
    if (counter % 2 == 0){
      if(this->gpio_controller_.GetPinValue(L3) == gpio::Value::HIGH)
        this->gpio_controller_.SetPinValue(L3, gpio::Value::LOW);
      else
        this->gpio_controller_.SetPinValue(L3, gpio::Value::HIGH);
    }
    counter++;
  }
  return core::ErrorCode::kOk;
}
core::ErrorCode exampleApp::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
  AppLogger::Info("exampleApp::Initialize() called");
  this->gpio_controller_ = gpio::GPIOController(new com::soc::IpcSocket());
  this->gpio_controller_.Init(256);
  return core::ErrorCode::kOk;
}
}  // namespace example
}  // namespace simba