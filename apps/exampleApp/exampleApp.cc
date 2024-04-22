/***
 * @file exampleApp.cc
 * @author Jacek Kukawski (jacekka6@gmail.com)
 * @brief This is an example of how to use the Logger class.
 * @version 0.1
 * @date 2024-04-20
 * @copyright Copyright (c) 2024
*/
#include "apps/homework/exampleApp.h"
#include "core/logger/Logger.h"
namespace simba {
namespace example {
core::ErrorCode Test::Run(std::stop_token token) {
  AppLogger::Info("exampleApp::Run() called");
  while(true){
    AppLogger::Info("exampleApp::Run() running");
  }
  return core::ErrorCode::kOk;
}
core::ErrorCode Test::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
  AppLogger::Info("exampleApp::Initialize() called");
  return core::ErrorCode::kOk;
}
}  // namespace example
}  // namespace simba