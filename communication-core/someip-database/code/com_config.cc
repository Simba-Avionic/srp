/**
 * @file com_config.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "communication-core/someip-database/code/com_config.h"

#include <utility>

#include "core/logger/Logger.h"

namespace srp {
namespace com {
namespace config {

std::shared_ptr<someip::objects::ConfigDb> ComConfig::config_db = nullptr;

std::optional<someip::objects::Endpoint> ComConfig::FindObject(
    const std::string& key) {
  if (config_db) {
    return config_db->FindObject(key);
  }
  return {};
}
void ComConfig::SetConfigDb(std::shared_ptr<someip::objects::ConfigDb> config) {
  config_db = std::move(config);
}
void ComConfig::InitConfigDb(const std::string& path) {
  std::ifstream f(path);
  if (f.is_open()) {
    AppLogger::Info("Config DB initialization");
    auto obj = std::make_shared<someip::objects::ConfigDb>();
    auto json_obj = nlohmann::json::parse(f);
    someip::json::ConfigDbParser::ParseJsonObject(obj, json_obj);
    ComConfig::SetConfigDb(obj);
  }
}
}  // namespace config
}  // namespace com
}  // namespace srp
