/**
 * @file em_service.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "platform/common/em/code/services/em/em_service.h"

#include <bits/stdc++.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>

#include "ara/log/log.h"
#include "core/json/json_parser.h"

namespace simba {
namespace em {
namespace service {

EmService::EmService(/* args */) {}

EmService::~EmService() {}

bool EmService::IsSrpApp(const std::string& path) noexcept {
  std::ifstream file{path + "/etc/srp_app.json"};
  return file.is_open();
}

void EmService::LoadApps() noexcept {
  try {
    for (auto& p : std::filesystem::directory_iterator("/opt")) {
      if (p.is_directory() && p.path() != "/opt/em") {
        if (this->IsSrpApp(p.path().c_str())) {
          std::string pp{p.path().string() + "/etc/srp_app.json"};
          auto res = this->GetAppConfig(pp);
          if (res.has_value()) {
            this->app_list.push_back(res.value());
            if (std::find(this->app_level_list.begin(),
                          this->app_level_list.end(),
                          res.value().GetStartUpPrio()) ==
                this->app_level_list.end()) {
              this->app_level_list.push_back(res.value().GetStartUpPrio());
            }

            ara::log::LogInfo() << "App: " << res.value().GetBinPath()
                                << " added to boot list with prio: "
                                << std::to_string(res.value().GetStartUpPrio());
          }
        }
      }
    }
  } catch (std::exception& e) {
    ara::log::LogError() << e.what();
  }
}

std::optional<data::AppConfig> EmService::GetAppConfig(
    const std::string& path) noexcept {
  auto obj = core::json::JsonParser::Parser(path).value();
  std::string bin_path{""};
  std::string parm{""};
  uint8_t prio{0};
  uint8_t delay{0};
  uint8_t error_count{0};
  uint16_t app_id{0};
  {
    auto bin_path_r = obj.GetString("bin_path");
    if (bin_path_r.has_value()) {
      bin_path = bin_path_r.value();
    } else {
      ara::log::LogError() << "Application from: " << path
                           << ", don't have: bin_path";
      error_count++;
    }
  }
  {
    auto parm_r = obj.GetString("parms");
    if (parm_r.has_value()) {
      parm = parm_r.value();
    } else {
      ara::log::LogError() << "Application from: " << path
                           << ", don't have: parms";
      error_count++;
    }
  }
  {
    auto prio_r = obj.GetNumber<uint8_t>("startup_prio");
    if (prio_r.has_value()) {
      prio = prio_r.value();
    } else {
      ara::log::LogError() << "Application from: " << path
                           << ", don't have: startup_prio";
      error_count++;
    }
  }
  {
    auto delay_r = obj.GetNumber<uint8_t>("startup_after_delay");
    if (delay_r.has_value()) {
      delay = delay_r.value();
    } else {
      ara::log::LogError() << "Application from: " << path
                           << ", don't have: startup_after_delay";
      error_count++;
    }
  }
  if (error_count != 0) {
    return {};
  } else {
    return std::optional{data::AppConfig{bin_path, parm, prio, delay}};
  }
}
void EmService::StartApps() noexcept {
  std::sort(this->app_level_list.begin(), this->app_level_list.end(),
            std::greater<uint8_t>());
  ara::log::LogInfo() << "Apps starting";
  for (const auto& level : app_level_list) {
    this->StartApps(level);
  }
  ara::log::LogInfo() << "Apps started";
}

void EmService::StartApps(const uint8_t level) noexcept {
  for (auto& app : app_list) {
    if (app.GetStartUpPrio() == level) {
      pid_t pid = this->StartApp(app);
      if (pid == 0) {
        ara::log::LogError() << "Failed to start app";
        return;
      }
      app.SetPid(pid);
      /**todo Dodać sleep  w  gdy serwis oczekuje czekania*/
    }
  }
}

std::optional<pid_t> EmService::RestartApp(const uint16_t appID) {
  // for (auto& app : this->app_list) {
  //   if (app.GetAppID() == appID) {
  //     if (kill(app.GetPid(), SIGKILL) != 0) {
  //       return {};
  //     }
  //     int status;
  //     uint8_t i = 0;
  //     do {
  //       waitpid(app.GetPid(), &status, 0);
  //       i++;
  //     } while (status == -1 && i < 5);
  //     if (status == -1) {
  //       ara::log::LogWarn() << "App with PID: " <<
  //       std::to_string(app.GetPid())
  //                           << "change  to zombie";
  //       return {};
  //     }
  //     pid_t pid = this->StartApp(app);
  //     if (pid == 0) {
  //       ara::log::LogError() << "Failed to start app";
  //       return {};
  //     }
  //     app.SetPid(pid);
  //     return std::optional<pid_t>{pid};
  //   }
  // }
}

pid_t EmService::StartApp(const simba::em::service::data::AppConfig& app) {
  pid_t pid{0};
  posix_spawnattr_t attr;
  posix_spawnattr_init(&attr);
  posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETPGROUP);
  posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSID);
  auto path = app.GetBinPath();
  auto parms = app.GetParms();
  char* argv[] = {path.data(), parms.data(), NULL};
  posix_spawnp(&pid, app.GetBinPath().c_str(), NULL, &attr, argv, NULL);
  ara::log::LogInfo() << "Spawning app: " << app.GetBinPath()
                      << " pid: " << std::to_string(pid);
  return pid;
}

}  // namespace service
}  // namespace em
}  // namespace simba
