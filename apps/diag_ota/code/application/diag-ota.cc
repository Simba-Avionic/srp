/**
 * @file diag-ota.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "apps/diag_ota/code/application/diag-ota.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <thread>  // NOLINT
#include <utility>
#include <vector>

#include "core/json/json_parser.h"
#include "core/logger/Logger.h"
#include "apps/diag_ota/code/application/proxy/read_vin_did.h"
#include "apps/diag_ota/code/application/proxy/uptime_did.h"
namespace simba {
namespace diag_ota {

/**
 * @brief This function is called to launch the application
 *
 * @param token stop token
 */
core::ErrorCode DiagOta::Run(std::stop_token token) {
  AppLogger::Info("Ecu id: " + std::to_string(ecu_id));
  AppLogger::Info("Eng token: " + std::to_string(eng_token));
  AppLogger::Info("Plant token: " + std::to_string(plant_token));
  AppLogger::Info("ECU MODE token: " + std::to_string(mode));
  if (eng_token == mode) {
    AppLogger::Info("ECU MODE: Eng mode");
    mode_event->SetValue({0x01});
  } else {
    AppLogger::Info("ECU MODE: Plant mode");
    mode_event->SetValue({0x00});
  }
  if (uds_server) {
    uds_server->Start();
  }
  read_vin_->StartService();
  uptime_->StartService();
  // diag_controller.Read(1000, 10);
  this->SleepMainThread();
}
/**
 * @brief This function is called to initialiaze the application
 *
 * @param parms map with parms
 */
core::ErrorCode DiagOta::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  auto obj_r = core::json::JsonParser::Parser("/opt/" + parms.at("app_name") +
                                              "/etc/config.json");
  if (!obj_r.has_value()) {
    AppLogger::Error("File not found: /opt/" + parms.at("app_name") +
                     "/etc/config.json");
    return core::kError;
  }
  auto json_obj = obj_r.value();
  {
    auto ip_t = json_obj.GetNumber<uint32_t>("ecu_id");
    if (!ip_t.has_value()) {
      AppLogger::Error("ecu_id not found in config file");
      return core::kError;
    }
    ecu_id = ip_t.value();
  }
  {
    auto ip_t = json_obj.GetNumber<uint32_t>("eng_token");
    if (!ip_t.has_value()) {
      AppLogger::Error("eng_token not found in config file");
      return core::kError;
    }
    eng_token = ip_t.value();
  }
  {
    auto ip_t = json_obj.GetNumber<uint32_t>("plant-token");
    if (!ip_t.has_value()) {
      AppLogger::Error("plant-token not found in config file");
      return core::kError;
    }
    plant_token = ip_t.value();
  }
  {
    auto ip_t = json_obj.GetNumber<uint32_t>("current_mode");
    if (!ip_t.has_value()) {
      AppLogger::Error("current_mode not found in config file");
      return core::kError;
    }
    mode = ip_t.value();
  }

  {
    uint16_t logic_id{0};
    std::string vin{""};
    auto logic_id_t = json_obj.GetNumber<uint16_t>("logic_address");
    if (logic_id_t.has_value()) {
      logic_id = logic_id_t.value();
    }

    auto vin_t = json_obj.GetString("vin");
    if (vin_t.has_value()) {
      vin = vin_t.value();
      AppLogger::Info("VIN: " + vin);
    }

    if (logic_id != 0x0000 & vin.length() > 0 && vin.length() < 18) {
      uds_server = std::make_unique<uds::UdsServer>(logic_id, vin);
    } else {
      AppLogger::Error("Wrong parms for UDS");
    }
  }

  if (this->com->GetServiceId() == 512) {
    mode_event = std::make_shared<com::someip::EventSkeleton>(
        "FC_DiagOtaApp/currentMode");
    this->com->Add(mode_event);
  } else {
    mode_event = std::make_shared<com::someip::EventSkeleton>(
        "EC_DiagOtaApp/currentMode");
    this->com->Add(mode_event);
  }
  this->read_vin_ = std::make_unique<ReadVinDiD>("EC_DiagOtaApp/vin");
  this->uptime_ = std::make_unique<UptimeDiD>("EC_DiagOtaApp/uptime_did");
}

}  // namespace diag_ota
}  // namespace simba
