/**
 * @file dlt_service.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "mw/logger/code/application/dlt_service.h"

#include <memory>
#include <vector>

#include "communication-core/sockets/socket_config.h"
#include "core/json/json_parser.h"
#include "core/logger/Logger.h"
#include "mw/logger/code/data/dlt_frame.h"
#include "mw/logger/code/data/dlt_log_type.h"
#include "mw/logger/code/data/dlt_string.h"
namespace simba {
namespace mw {
namespace dlt {

core::ErrorCode DltService::Run(std::stop_token token) {
  std::this_thread::sleep_for(std::chrono::seconds{5});
  while (!token.stop_requested()) {
    auto res_v = this->logs.GetWithoutRemove();
    auto res = res_v->ParseFrame();
    if (soc.Transmit("231.255.42.99", tx_port, res.value()) ==
        core::ErrorCode::kOk) {
      this->logs.Remove();
    }
  }
  AppLogger::Info("Stop");
  return core::kOk;
}

core::ErrorCode DltService::Initialize(
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
    auto ip_t = json_obj.GetString("ip");
    if (!ip_t.has_value()) {
      AppLogger::Error("Ip not found in config file");
      return core::kError;
    }
    ip_ = ip_t.value();
  }
  {
    auto port_t = json_obj.GetNumber<uint16_t>("tx_port");
    if (!port_t.has_value()) {
      AppLogger::Error("tx port not found in config file");
      return core::kError;
    }
    tx_port = port_t.value();
  }
  {
    auto ip_t = json_obj.GetString("ecu_id");
    if (!ip_t.has_value()) {
      AppLogger::Error("Ecu id not found in config file");
      return core::kError;
    }
    ec_name = ip_t.value();
  }
  AppLogger::Info("Config loaded: ip=" + ip_ +
                  ", port=" + std::to_string(tx_port) + ", name=" + ec_name);
  this->InitUDP();
  this->InitIPC();

  return core::kOk;
}

DltService::DltService(/* args */) {}

DltService::~DltService() {}

void DltService::InitIPC() noexcept {
  if (ipc_soc.Init(com::soc::SocketConfig{"SIMBA.DLT", 0, 0}) ==
      core::ErrorCode::kOk) {
    ipc_soc.SetRXCallback([this](const std::string& ip,
                                 const std::uint16_t& port,
                                 std::vector<std::uint8_t> payload) {
      const uint8_t mode = payload.at(0);
      simba::dlt::data::DLTLogType types =
          simba::dlt::data::DLTLogType::kDLTDebug;
      switch (mode) {
        case 0:
          types = simba::dlt::data::DLTLogType::kDLTDebug;
          break;
        case 1:
          types = simba::dlt::data::DLTLogType::kDLTInfo;
          break;
        case 2:
          types = simba::dlt::data::DLTLogType::kDLTWarning;
          break;
        case 3:
          types = simba::dlt::data::DLTLogType::kDLTError;
          break;
        default:
          break;
      }
      uint32_t timestamp{0};
      std::memcpy(&timestamp, (payload.data() + 1), sizeof(uint32_t));
      const std::string app_id{payload.begin() + 5, payload.begin() + 9};
      const std::string log_content{payload.begin() + 9, payload.end()};
      auto r = std::make_shared<
          simba::dlt::data::DltFrame<simba::dlt::data::DltString>>(
          timestamp, this->ec_name, app_id, types,
          simba::dlt::data::DltString{log_content});
      if (!this->logs.push(r)) {
        AppLogger::Error("DLT msg dropped in TX queue!!");
      }
    });
    ipc_soc.StartRXThread();
    AppLogger::Info("IPC socket started");
  }
}

void DltService::InitUDP() noexcept {
  const com::soc::SocketConfig conf{ip_, tx_port, 0};

  if (soc.Init(conf) != core::ErrorCode::kOk) {
    AppLogger::Error("UDP socket error");
  }
  AppLogger::Info("UDP socket started");
}

}  // namespace dlt
}  // namespace mw
}  // namespace simba
