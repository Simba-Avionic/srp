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

#include "platform/common/logger/code/application/dlt_service.h"

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "ara/log/log.h"
#include "core/json/json_parser.h"
#include "platform/common/logger/code/data/dlt_frame.h"
#include "platform/common/logger/code/data/dlt_log_type.h"
#include "platform/common/logger/code/data/dlt_string.h"
namespace srp {
namespace dlt {

int DltService::Run(const std::stop_token &token) {
  while (!token.stop_requested()) {
    auto res_v = this->logs.GetWithoutRemove(token);
    if (token.stop_requested()) {
      return 0;
    }
    auto res = res_v.value()->ParseFrame();
    soc.Transmit(res.value());
    this->logs.Remove();
  }
  this->ipc_soc.StopRXThread();
  ara::log::LogInfo() << "Stop";
  return 0;
}

int DltService::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  auto obj_r = core::json::JsonParser::Parser(
      std::string{"/srp/opt/cpu_srp/logger_config.json"});
  if (!obj_r.has_value()) {
    ara::log::LogError() << "File not found: /opt/cpu_srp/logger_config.json";
    return -1;
  }
  auto json_obj = obj_r.value();
  {
    auto ip_t = json_obj.GetString("ip");
    if (!ip_t.has_value()) {
      ara::log::LogError() << "Ip not found in config file";
      return -1;
    }
    ip_ = ip_t.value();
  }
  {
    auto port_t = json_obj.GetNumber<uint16_t>("tx_port");
    if (!port_t.has_value()) {
      ara::log::LogError() << "tx port not found in config file";
      return -1;
    }
    tx_port = port_t.value();
  }
  {
    auto ip_t = json_obj.GetString("ecu_id");
    if (!ip_t.has_value()) {
      ara::log::LogError() << "Ecu id not found in config file";
      return -1;
    }
    ec_name = ip_t.value();
  }
  ara::log::LogInfo() << "Config loaded: ip=" << ip_
                      << ", port=" << std::to_string(tx_port)
                      << ", name=" << ec_name;
  this->InitUDP();
  this->InitIPC();

  return 0;
}

DltService::DltService(/* args */) {}

DltService::~DltService() {
  // ipc_soc.StopRXThread();
}

void DltService::InitIPC() noexcept {
  if (ipc_soc.Init(com::soc::SocketConfig{"SRP.ARA.DLT", 0, 0}) ==
      core::ErrorCode::kOk) {
    ipc_soc.SetRXCallback([this](const std::string &ip,
                                 const std::uint16_t &port,
                                 std::vector<std::uint8_t> payload) {
      if (payload.size() > 13) {
        const uint8_t mode = payload.at(0);
        srp::dlt::data::DLTLogType types =
            srp::dlt::data::DLTLogType::kDLTDebug;
        switch (mode) {
          case 0:
            types = srp::dlt::data::DLTLogType::kDLTVerbose;
            break;
          case 1:
            types = srp::dlt::data::DLTLogType::kDLTDebug;
            break;
          case 2:
            types = srp::dlt::data::DLTLogType::kDLTInfo;
            break;
          case 3:
            types = srp::dlt::data::DLTLogType::kDLTWarning;
            break;
          case 4:
            types = srp::dlt::data::DLTLogType::kDLTError;
            break;
          case 5:
            types = srp::dlt::data::DLTLogType::kDLTFatal;
            break;
          default:
            break;
        }
        uint32_t timestamp{0};
        std::memcpy(&timestamp, (payload.data() + 1), sizeof(uint32_t));
        const std::string app_id{payload.begin() + 5, payload.begin() + 9};
        const std::string ctx_id{payload.begin() + 9, payload.begin() + 13};
        const std::string log_content{payload.begin() + 13, payload.end()};
        auto r = std::make_shared<
            srp::dlt::data::DltFrame<srp::dlt::data::DltString>>(
            timestamp, this->ec_name, app_id, ctx_id, types,
            srp::dlt::data::DltString{log_content});
        if (!this->logs.push(std::move(r))) {
          ara::log::LogError() << "DLT msg dropped in TX queue!!";
        }
      }
    });
    ipc_soc.StartRXThread();
    ara::log::LogInfo() << "IPC socket started";
  }
}

void DltService::InitUDP() noexcept {
  if (soc.Init(ip_, "231.255.42.99", tx_port) != core::ErrorCode::kOk) {
    ara::log::LogError() << "UDP socket error";
  }
  ara::log::LogInfo() << "UDP socket started";
}

}  // namespace dlt
}  // namespace srp
