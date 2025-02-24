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
#include "mw/em/code/services/em/em_service.h"

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

void run_cmd(char* cmd) {
  pid_t pid;
  char* argv[] = {"example", NULL};
  int status;
  printf("Run command: %s\n", cmd);
  status =
      posix_spawn(&pid, "/opt/example/bin/example", NULL, NULL, argv, NULL);
  setsid();
  if (status == 0) {
    printf("Child pid: %i\n", pid);
  } else {
    printf("posix_spawn: %s\n", strerror(status));
  }
}
// #include "boost/process.hpp"
#include "core/json/json_parser.h"
namespace srp {
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
          }
        }
      }
    }
  } catch (std::exception& e) {
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
      error_count++;
    }
  }
  {
    auto parm_r = obj.GetString("parms");
    if (parm_r.has_value()) {
      parm = parm_r.value();
    } else {
      error_count++;
    }
  }
  {
    auto prio_r = obj.GetNumber<uint8_t>("startup_prio");
    if (prio_r.has_value()) {
      prio = prio_r.value();
    } else {
      error_count++;
    }
  }
  {
    auto delay_r = obj.GetNumber<uint8_t>("startup_after_delay");
    if (delay_r.has_value()) {
      delay = delay_r.value();
    } else {
      error_count++;
    }
    auto app_id_r = obj.GetNumber<uint16_t>("app_id");
    if (!app_id_r.has_value()) {
      error_count++;
    } else {
      app_id = app_id_r.value();
    }
  }
  if (error_count != 0) {
    return std::nullopt;
  } else {
    return std::optional{data::AppConfig{bin_path, parm, prio, delay, app_id}};
  }
}
void EmService::StartApps() noexcept {
  std::sort(this->app_level_list.begin(), this->app_level_list.end(),
            std::greater<uint8_t>());
  for (const auto& level : app_level_list) {
    this->StartApps(level);
  }
}

void EmService::StartApps(const uint8_t level) noexcept {
  for (auto& app : app_list) {
    if (app.GetStartUpPrio() == level) {
      pid_t pid = this->StartApp(app);
      if (pid == 0) {
        return;
      }
      app.SetPid(pid);
      /**todo Dodać sleep  w  gdy serwis oczekuje czekania*/
    }
  }
}

std::optional<pid_t> EmService::RestartApp(const uint16_t appID) {
  for ( auto &app : this->app_list ) {
    if ( app.GetAppID() == appID ) {
      if (kill(app.GetPid(), SIGKILL) != 0) {
        return std::nullopt;
      }
      int status;
      uint8_t i = 0;
      do {
        waitpid(app.GetPid(), &status, 0);
        i++;
      } while (status == -1 && i < 5);
      if (status == -1) {
        return std::nullopt;
      }
      pid_t pid = this->StartApp(app);
      if (pid == 0) {
        return std::nullopt;
      }
      app.SetPid(pid);
      return std::optional<pid_t>{pid};
    }
  }
}

pid_t EmService::StartApp(const srp::em::service::data::AppConfig &app) {
    pid_t pid{0};
    char* app_args = new char[app.GetBinPath().size()];
    sprintf(app_args, "%s", app.GetBinPath().c_str());  // NOLINT
    char* argv[] = {app_args, NULL};
    posix_spawn(&pid, app.GetBinPath().c_str(), NULL, NULL, argv, NULL);
    delete[] app_args;
    return pid;
  }

}  // namespace service
}  // namespace em
}  // namespace srp
