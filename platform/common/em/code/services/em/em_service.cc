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
#include <memory>
#include <string>

#include "ara/log/log.h"
#include "platform/common/em/code/services/em/json_parser.h"

namespace srp {
namespace em {
namespace service {

EmService::EmService(
    std::shared_ptr<data::IAppDb> db,
    const std::function<void(const uint16_t&)>&& update_callback)
    : db_{db}, update_callback_(std::move(update_callback)) {}

EmService::~EmService() {}

bool EmService::IsSrpApp(const std::string& path) noexcept {
  std::ifstream file{path + "/etc/srp_app.json"};
  return file.is_open();
}

void EmService::LoadApps() noexcept {
  try {
    for (auto& p : std::filesystem::directory_iterator("/srp/opt")) {
      if (p.is_directory()) {
        if (this->IsSrpApp(p.path().c_str())) {
          std::string pp{p.path().string() + "/etc/srp_app.json"};
          auto res = json::JsonParser::GetAppConfig(pp);
          if (res.has_value()) {
            if (db_->InsertNewApp(res.value()) == 0) {
              ara::log::LogInfo()
                  << "App: " << res.value().GetAppName() << " added to db";
            }
          }
        }
      }
    }
  } catch (std::exception& e) {
    ara::log::LogError() << e.what();
  }
}

void EmService::SetActiveState(const uint16_t& state_id_) noexcept {
  std::vector<uint16_t> terminate_list{};
  const auto currect_list_opt = db_->GetFgAppList(active_state);
  const auto next_list_opt = db_->GetFgAppList(state_id_);
  if (!next_list_opt.has_value()) {
    ara::log::LogError() << "State: " << state_id_ << " not supported!";
    return;
  }
  const auto& next_list = next_list_opt.value().get();
  if (currect_list_opt.has_value()) {
    const auto& currect_list = currect_list_opt.value().get();
    for (const auto& app_id : currect_list) {
      if (!next_list.contains(app_id)) {
        terminate_list.push_back(app_id);
      }
    }
  }
  // TODO(bartek): kill app
  for (const auto& app_id_ : next_list) {
    auto app_config_opt = db_->GetAppConfig(app_id_);
    if (app_config_opt.has_value()) {
      const auto& app_config = app_config_opt.value().get();
      if (app_config.GetPid() == 0) {
        const auto new_pid = this->StartApp(app_config);
        db_->SetPidForApp(app_id_, new_pid);
      }
    }
  }
  if (update_callback_) {
    update_callback_(state_id_);
  }
  this->active_state = state_id_;
}

std::optional<pid_t> EmService::RestartApp(const uint16_t appID) {
  return std::nullopt;
}

pid_t EmService::StartApp(const srp::em::service::data::AppConfig& app) {
  pid_t pid{0};
  posix_spawnattr_t attr;
  posix_spawnattr_init(&attr);
  posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETPGROUP);
  posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSID);
  auto path = app.GetBinPath();
  auto parms = app.GetParms();
  char* argv[] = {path.data(), parms.data(), NULL};
  posix_spawnp(&pid, app.GetBinPath().c_str(), NULL, &attr, argv, NULL);
  ara::log::LogInfo() << "Spawning app: " << app.GetAppName()
                      << " pid: " << std::to_string(pid);
  return pid;
}

}  // namespace service
}  // namespace em
}  // namespace srp
