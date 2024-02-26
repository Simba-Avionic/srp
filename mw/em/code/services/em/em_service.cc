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
#include "core/logger/Logger.h"
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
          if (res.HasValue()) {
            this->app_list.push_back(res.Value());
            if (std::find(this->app_level_list.begin(),
                          this->app_level_list.end(),
                          res.Value().GetStartUpPrio()) ==
                this->app_level_list.end()) {
              this->app_level_list.push_back(res.Value().GetStartUpPrio());
            }
            AppLogger::Info("App: " + res.Value().GetBinPath() +
                            " added to boot list with prio: " +
                            std::to_string(res.Value().GetStartUpPrio()));
          }
        }
      }
    }
  } catch (std::exception& e) {
    AppLogger::Error(e.what());
  }
}

core::Result<data::AppConfig> EmService::GetAppConfig(
    const std::string& path) noexcept {
  auto obj = core::json::JsonParser::Parser(path).Value();
  std::string bin_path{""};
  std::string parm{""};
  uint8_t prio{0};
  uint8_t delay{0};
  uint8_t error_count{0};
  {
    auto bin_path_r = obj.GetString("bin_path");
    if (bin_path_r.HasValue()) {
      bin_path = bin_path_r.Value();
    } else {
      AppLogger::Error("Application from: " + path + ", don't have: bin_path");
      error_count++;
    }
  }
  {
    auto parm_r = obj.GetString("parms");
    if (parm_r.HasValue()) {
      parm = parm_r.Value();
    } else {
      AppLogger::Error("Application from: " + path + ", don't have: parms");
      error_count++;
    }
  }
  {
    auto prio_r = obj.GetNumber<uint8_t>("startup_prio");
    if (prio_r.HasValue()) {
      prio = prio_r.Value();
    } else {
      AppLogger::Error("Application from: " + path +
                       ", don't have: startup_prio");
      error_count++;
    }
  }
  {
    auto delay_r = obj.GetNumber<uint8_t>("startup_after_delay");
    if (delay_r.HasValue()) {
      delay = delay_r.Value();
    } else {
      AppLogger::Error("Application from: " + path +
                       ", don't have: startup_after_delay");
      error_count++;
    }
  }
  if (error_count != 0) {
    return core::Result<data::AppConfig>{};
  } else {
    return core::Result{data::AppConfig{bin_path, parm, prio, delay}};
  }
}
void EmService::StartApps() noexcept {
  std::sort(this->app_level_list.begin(), this->app_level_list.end(),
            std::greater<uint8_t>());
  AppLogger::Info("Apps starting");
  for (const auto& level : app_level_list) {
    this->StartApps(level);
  }
  AppLogger::Info("Apps started");
}

void EmService::StartApps(const uint8_t level) noexcept {
  for (const auto& app : app_list) {
    if (app.GetStartUpPrio() == level) {
      pid_t pid;
      char* app_args = new char[app.GetBinPath().size()];
      sprintf(app_args, "%s", app.GetBinPath().c_str());  // NOLINT
      char* argv[] = {app_args, NULL};
      int status{0};
      status =
          posix_spawn(&pid, app.GetBinPath().c_str(), NULL, NULL, argv, NULL);
      delete[] app_args;
      AppLogger::Info("Spawning app: " + app.GetBinPath() +
                      " pid: " + std::to_string(pid));
      /**todo Dodać sleep  w  gdy serwis oczekuje czekania*/
    }
  }
}
}  // namespace service
}  // namespace em
}  // namespace simba
