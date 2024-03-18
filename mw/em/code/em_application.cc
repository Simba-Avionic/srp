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
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    AppLogger::Info("check if restart needed");
    auto res = this->exec_service.CheckAppCondition();
    while ( res.size() > 0 ) {
      AppLogger::Error("Restart needed");
      //this->em_service.RestartApp(res.front());
      AppLogger::Error("RESTARTED APP WITH ID: "+std::to_string(res.front()));
      res.pop();
    }
    }
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
  this->exec_service.Init();
  return core::ErrorCode::kOk;
}

}  // namespace em
}  // namespace simba
