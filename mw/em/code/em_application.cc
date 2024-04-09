/**
 * @file em_application.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "mw/em/code/em_application.h"

#include <string>
#include <unordered_map>
namespace simba {
namespace em {
EmApplication::EmApplication(/* args */) {}

EmApplication::~EmApplication() {}

core::ErrorCode EmApplication::Run(std::stop_token token) {
  this->em_service.StartApps();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  this->exec_service.Init();
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    auto res = this->exec_service.CheckAppCondition();
    while ( res.size() > 0 ) {
      auto pid = this->em_service.RestartApp(res.front());
      if ( !pid.has_value() ) {
        AppLogger::Error("ERROR RESTART APP WITH ID: "+std::to_string(res.front()));
      } else {
        AppLogger::Error("RESTARTED APP WITH ID: "+std::to_string(res.front()));
        this->exec_service.RestartedApp(res.front());
        res.pop();
      }
    }
  }
  this->SleepMainThread();
  return core::ErrorCode::kOk;
}
/**
 * @brief This function is called to initialize the application
 *
 * @param parms map with parms
 */
core::ErrorCode EmApplication::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  this->em_service.LoadApps();
  this->exec_service.SetApps(this->em_service.GetAppList());
  return core::ErrorCode::kOk;
}

}  // namespace em
}  // namespace simba
