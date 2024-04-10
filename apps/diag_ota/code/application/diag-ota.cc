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
#include <utility>
#include <vector>

#include "core/json/json_parser.h"
#include "core/logger/Logger.h"

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
  diag_controller.Read(1000, 10);
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
  if (this->com->GetServiceId() == 512) {
    mode_event = std::make_shared<com::someip::EventSkeleton>(
        "FC_DiagOtaApp/currentMode");
    write_someip = std::make_shared<com::someip::MethodSkeleton>(
        "FC_DiagOtaApp/diagMethodRequestWrite",
        std::bind(&DiagOta::WriteSomeip, this, std::placeholders::_1));
    read_someip = std::make_shared<com::someip::MethodSkeleton>(
        "FC_DiagOtaApp/diagMethodRequestRead",
        std::bind(&DiagOta::ReadSomeip, this, std::placeholders::_1));
    job_someip = std::make_shared<com::someip::MethodSkeleton>(
        "FC_DiagOtaApp/diagMethodRequestJob",
        std::bind(&DiagOta::JobSomeip, this, std::placeholders::_1));
    this->com->Add(mode_event);
    this->com->Add(write_someip);
    this->com->Add(read_someip);
    this->com->Add(job_someip);
  } else {
    mode_event = std::make_shared<com::someip::EventSkeleton>(
        "EC_DiagOtaApp/currentMode");
    write_someip = std::make_shared<com::someip::MethodSkeleton>(
        "EC_DiagOtaApp/diagMethodRequestWrite",
        std::bind(&DiagOta::WriteSomeip, this, std::placeholders::_1));
    read_someip = std::make_shared<com::someip::MethodSkeleton>(
        "EC_DiagOtaApp/diagMethodRequestRead",
        std::bind(&DiagOta::ReadSomeip, this, std::placeholders::_1));
    job_someip = std::make_shared<com::someip::MethodSkeleton>(
        "EC_DiagOtaApp/diagMethodRequestJob",
        std::bind(&DiagOta::JobSomeip, this, std::placeholders::_1));
    this->com->Add(mode_event);
    this->com->Add(write_someip);
    this->com->Add(read_someip);
    this->com->Add(job_someip);
  }

  this->someip_diag_controller = std::make_unique<diag::DiagController>(
      0x00, std::make_unique<com::soc::IpcSocket>());
  this->someip_diag_controller->Init();
}

std::optional<std::vector<uint8_t>> DiagOta::ReadSomeip(
    const std::vector<uint8_t> payload) {
  if (payload.size() != 3U) {
    return {};
  }
  uint16_t s_id{payload[0] << 1 + payload[2]};
  return someip_diag_controller->Read(s_id, payload[3]);
}
std::optional<std::vector<uint8_t>> DiagOta::JobSomeip(
    const std::vector<uint8_t> payload) {
  if (payload.size() < 3U) {
    return {};
  }
  std::vector<uint8_t> pp{};
  uint16_t s_id{payload[0] << 1 + payload[2]};
  std::copy(payload.begin() + 3, payload.end(), std::back_inserter(pp));
  return someip_diag_controller->Job(s_id, payload[3], std::move(pp));
}
std::optional<std::vector<uint8_t>> DiagOta::WriteSomeip(
    const std::vector<uint8_t> payload) {
  if (payload.size() < 3U) {
    return {};
  }
  std::vector<uint8_t> pp{};
  uint16_t s_id{payload[0] << 1 + payload[2]};
  std::copy(payload.begin() + 3, payload.end(), std::back_inserter(pp));
  if (someip_diag_controller->Write(s_id, payload[3], std::move(pp)) ==
      core::ErrorCode::kOk) {
    return std::vector<uint8_t>{};
  } else {
    return {};
  }
}

}  // namespace diag_ota
}  // namespace simba
