
/**
 * @file diag_demon.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "platform/common/diag_demon/code/application/diag_demon.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <thread>  // NOLINT
#include <utility>
#include <vector>

#include "ara/log/log.h"
#include "core/common/condition.h"
#include "core/json/json_parser.h"
#include "platform/common/diag_demon/code/services/example_did.h"

namespace simba {
namespace platform {
namespace diag_demon {

/**
 * @brief This function is called to launch the application
 *
 * @param token stop token
 */
int DiagDemon::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "Ecu id: " << std::to_string(ecu_id);
  ara::log::LogInfo() << "Eng token: " << std::to_string(eng_token);
  ara::log::LogInfo() << "Plant token: " << std::to_string(plant_token);
  ara::log::LogInfo() << "ECU MODE token: " << std::to_string(mode);
  if (eng_token == mode) {
    ara::log::LogInfo() << "ECU MODE: Eng mode";
  } else {
    ara::log::LogInfo() << "ECU MODE: Plant mode";
  }
  if (uds_server) {
    uds_server->Start();
  }
  this->example_did->StartOffer();
  core::condition::wait(token);
  this->example_did->StopOffer();
  uds_server->Stop();
  return 0;
}
/**
 * @brief This function is called to initialiaze the application
 *
 * @param parms map with parms
 */
int DiagDemon::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  auto obj_r = core::json::JsonParser::Parser(
      std::string{"/opt/cpu_simba/diag_config.json"});
  if (!obj_r.has_value()) {
    ara::log::LogError() << "File not found: /opt/cpu_simba/diag_config.json";
    return core::kError;
  }
  auto json_obj = obj_r.value();
  {
    auto ip_t = json_obj.GetNumber<uint32_t>("ecu_id");
    if (!ip_t.has_value()) {
      ara::log::LogError() << "ecu_id not found in config file";
      return core::kError;
    }
    ecu_id = ip_t.value();
  }
  {
    auto ip_t = json_obj.GetNumber<uint32_t>("eng_token");
    if (!ip_t.has_value()) {
      ara::log::LogError() << "eng_token not found in config file";
      return core::kError;
    }
    eng_token = ip_t.value();
  }
  {
    auto ip_t = json_obj.GetNumber<uint32_t>("plant-token");
    if (!ip_t.has_value()) {
      ara::log::LogError() << "plant-token not found in config file";
      return core::kError;
    }
    plant_token = ip_t.value();
  }
  {
    auto ip_t = json_obj.GetNumber<uint32_t>("current_mode");
    if (!ip_t.has_value()) {
      ara::log::LogError() << "current_mode not found in config file";
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
      ara::log::LogInfo() << "VIN: " << vin;
    }

    if ((logic_id != 0x0000) && vin.length() > 0 && vin.length() < 18) {
      uds_server = std::make_unique<uds::UdsServer>(logic_id, vin);
    } else {
      ara::log::LogError() << "Wrong parms for UDS";
    }
  }
  this->example_did = std::make_unique<ExampleDiD>(
      ara::core::InstanceSpecifier{"/simba/platform/diag_demon/UDSReadVin"});
  return 0;
}
}  // namespace diag_demon
}  // namespace platform
}  // namespace simba
