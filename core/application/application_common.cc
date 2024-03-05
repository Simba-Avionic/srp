/**
 * @file application_common.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "core/application/application_common.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "boost/algorithm/string.hpp"
#include "communication-core/someip-database/code/com_config.h"
#include "communication-core/someip-database/code/database.h"
#include "communication-core/someip-database/code/database_parser.h"
#include "core/application/parm.h"
#include "core/json/json_parser.h"
#include "core/logger/Logger.h"
#include "core/logger/logger_factory.h"
#include "nlohmann/json.hpp"
namespace simba {
namespace core {
void ApplicationCommon::StopApp() {
  this->source.request_stop();
  // exit(1);
}
bool ApplicationCommon::FileExist(const std::string& name) {
  std::ifstream file{name};
  return file.is_open();
}
void ApplicationCommon::RunApp(int argc, char const* argv[]) {
  std::unordered_map<std::string, std::string> parms{};
  for (int i = 0; i < argc; i++) {
    Parm p{std::string{argv[i]}};
    parms.insert({p.GetName(), p.GetValue()});
  }
  const std::string help_path = parms.begin()->first;
  const std::string app_name =
      help_path.substr(help_path.find_last_of("/") + 1);
  parms.insert({"app_name", app_name});
  auto obj = json::JsonParser::Parser("/opt/" + parms.at("app_name") +
                                      "/etc/srp_app.json")
                 .value();
  auto app_id_ = obj.GetNumber<uint16_t>("app_id");
  if (app_id_.has_value()) {
    this->exec_.Init(app_id_.value());
  }
  onRun(parms);
  parms.clear();
}

void ApplicationCommon::SleepMainThread() {
  while (true) {
    std::this_thread::sleep_for(std::chrono::hours{30});
  }
}

void ApplicationCommon::onRun(
    const std::unordered_map<std::string, std::string>& parms) {
  if (this->CommonConfig(parms) != ErrorCode::kOk) {
    std::abort();
  }
  this->SomeIPConfig(parms);
  AppLogger::Info("Application initialize");
  this->Initialize(parms);
  AppLogger::Info("Application running");
  this->Run(this->source.get_token());
}

ErrorCode ApplicationCommon::CommonConfig(
    const std::unordered_map<std::string, std::string>& parms) {
  ErrorCode res = ErrorCode::kOk;

  std::ifstream file{"/opt/" + parms.at("app_name") + "/etc/logger.json"};
  if (!file.is_open()) {
    std::cerr << "Logger config file not exist! -> "
              << "/opt/" + parms.at("app_name") + "/etc/logger.json"
              << std::endl;
    return ErrorCode::kError;
  }
  nlohmann::json data = nlohmann::json::parse(file);
  std::string app_id{"NONE"};
  std::string app_desc{"NONE"};
  std::vector<std::string> log_mode{};
  logger::loggingLevel log_level{logger::loggingLevel::WARNING};
  if (data.contains("app_id")) {
    if (data.at("app_id").is_string()) {
      app_id = data.at("app_id");
    } else {
      res = ErrorCode::kError;
    }
  } else {
    res = ErrorCode::kError;
  }
  if (data.contains("app_des")) {
    if (data.at("app_des").is_string()) {
      app_desc = data.at("app_des");
    } else {
      res = ErrorCode::kError;
    }
  } else {
    res = ErrorCode::kError;
  }
  if (data.contains("log_level")) {
    if (data.at("log_level").is_string()) {
      const std::string temp = data.at("log_level");
      log_level = AppLogger::ConvertStringToLogLevel(temp);
    } else {
      res = ErrorCode::kError;
    }
  } else {
    res = ErrorCode::kError;
  }
  if (data.contains("log_mode")) {
    if (data.at("log_mode").is_string()) {
      const std::string temp = data.at("log_mode");
      boost::split(log_mode, temp, boost::is_any_of("|"));
    } else {
      res = ErrorCode::kError;
    }
  } else {
    res = ErrorCode::kError;
  }
  if (res == kOk) {
    AppLogger::SetParms(app_id, log_level);
    for (const auto& mode : log_mode) {
      auto res = logger::LoggerFactory::CreateLogger(mode);
      if (res.has_value()) {
        AppLogger::AddLogger(res.value());
      }
    }
  }
  AppLogger::Info("App id: " + app_id + ", Desc: " + app_desc);
  return res;
}

ErrorCode ApplicationCommon::SomeIPConfig(
    const std::unordered_map<std::string, std::string>& parms) {
  const std::string path{"/opt/" + parms.at("app_name") +
                         "/etc/app_someip.json"};
  com::config::ComConfig::InitConfigDb(path);
  std::ifstream f(path);
  if (f.is_open()) {
    AppLogger::Info("Config DB initialization");
    auto obj = std::make_shared<com::someip::objects::DataBase>();
    auto json_obj = nlohmann::json::parse(f);
    simba::com::someip::json::DataBaseParser::ParseJsonObject(obj, json_obj);
    this->com = std::make_unique<com::someip::Controller>(obj);
  }

  return ErrorCode::kOk;
}

ApplicationCommon::~ApplicationCommon() {}
}  // namespace core
}  // namespace simba
