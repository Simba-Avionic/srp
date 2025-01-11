/**
 * @file app_db.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "platform/common/em/code/services/em/app_db.h"

namespace srp {
namespace em {
namespace service {
namespace data {
AppDb::AppDb(/* args */) {}

int8_t AppDb::InsertNewApp(AppConfig app) noexcept {
  const auto res = this->app_list_.insert({app.GetAppId(), app}).second;
  for (const auto& fg : app.GetFg()) {
    const auto& iter = fg_name_2_id.find(fg);
    if (iter != fg_name_2_id.end()) {
      this->fg_list_[iter->second].insert(app.GetAppId());
    }
  }
  return res ? 0 : -1;
}
void AppDb::SetPidForApp(const uint16_t app_id, const uint32_t pid) noexcept {
  const auto& iter = this->app_list_.find(app_id);
  if (iter != this->app_list_.end()) {
    iter->second.SetPid(pid);
  }
}
int8_t AppDb::InsertNewFG(uint16_t fg_id, const std::string& name) noexcept {
  std::ignore = this->fg_name_2_id.insert({name, fg_id});
  const auto res = this->fg_list_.insert({fg_id, {}}).second;
  return res ? 0 : -1;
}

std::optional<std::reference_wrapper<const AppConfig>> AppDb::GetAppConfig(
    const uint16_t& app_id_) noexcept {
  const auto& iter = app_list_.find(app_id_);
  if (iter == app_list_.end()) {
    return std::nullopt;
  }

  return std::reference_wrapper<const AppConfig>{iter->second};
}
std::optional<std::reference_wrapper<const std::unordered_set<uint16_t>>>
AppDb::GetFgAppList(const uint16_t& fg_id) noexcept {
  const auto& iter = fg_list_.find(fg_id);
  if (iter == fg_list_.end()) {
    return std::nullopt;
  }

  return std::reference_wrapper<const std::unordered_set<uint16_t>>{
      iter->second};
}
}  // namespace data
}  // namespace service
}  // namespace em
}  // namespace srp
