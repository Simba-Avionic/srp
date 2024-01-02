/**
 * @file someip_controller.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "communication-core/someip-controller/someip_controller.h"

#include <string>

#include "communication-core/json-parser/database_json_parser.h"
#include "communication-core/someip/message_code.h"
#include "communication-core/someip/message_type.h"
#include "communication-core/someip/someip_header.h"
#include "core/results/result.h"
namespace simba {
namespace com {
namespace someip {
simba::core::Result<std::vector<uint8_t>> SomeIpController::Request(
    const uint16_t service_id, const uint16_t method_id,
    const std::vector<uint8_t> payload) {
  const auto service_data = db_.GetService(service_id);
  if (!service_data.HasValue()) {
    AppLogger::Error("[SOMEIPCONTROLLER] Service_id: " +
                     std::to_string(service_id) + " not found!");
    return simba::core::Result<std::vector<uint8_t>>{};
  } else {
    AppLogger::Error(
        "[SOMEIPCONTROLLER] Service_id: " + std::to_string(service_id) +
        " ip = " + service_data.Value().GetIp());
  }
  const auto transfer = GetTransferID();
  auto req = header_factory.CreateRequest(service_id, method_id);
  auto req_payload = msg_factory.GetBuffor(req, service_id_, transfer, payload);
  auto trans = this->AddTransfer(transfer);
  AppLogger::Debug("Sending to: " + std::to_string(req->GetServiceID()) +
                   " From " + std::to_string(req->GetClientID()));
  if (this->socket_->Transmit(service_data.Value().GetIp(),
                              service_data.Value().GetPort(),
                              req_payload) != simba::core::ErrorCode::kOk) {
    std::remove_if(
        this->transfers.begin(), transfers.end(),
        [trans](std::shared_ptr<simba::com::someip::data::Transfer> tr) {
          return tr->GetTransferID() == trans->GetTransferID();
        });
    AppLogger::Error(
        "[SOMEIPCONTROLLER] Request to :" + std::to_string(service_id) +
        " method_id: " + std::to_string(method_id) + " No connection");
    return simba::core::Result<std::vector<uint8_t>>{};
  }
  const auto res = trans->GetPayloadAsc();
  auto vec = trans->GetPayload();
  std::remove_if(
      this->transfers.begin(), transfers.end(),
      [trans](std::shared_ptr<simba::com::someip::data::Transfer> tr) {
        return tr->GetTransferID() == trans->GetTransferID();
      });
  if (res.HasValue()) {
    return simba::core::Result<std::vector<uint8_t>>{vec};
  } else {
    AppLogger::Error(
        "[SOMEIPCONTROLLER] Request to :" + std::to_string(service_id) + " : " +
        std::to_string(method_id) + " Timeout");
    return simba::core::Result<std::vector<uint8_t>>{};
  }
}

bool SomeIpController::RequestNoResponse(const uint16_t service_id,
                                         const uint16_t method_id,
                                         const std::vector<uint8_t> payload) {
  const auto service_data = db_.GetService(service_id);
  if (!service_data.HasValue()) {
    AppLogger::Error("[SOMEIPCONTROLLER] Service_id: " +
                     std::to_string(service_id) + " not found!");
    return false;
  }
  const auto transfer = GetTransferID();
  auto req = header_factory.CreateRequestNoReturn(service_id, method_id);
  auto req_payload =
      msg_factory.GetBuffor(req, this->service_id_, transfer, payload);
  auto trans = this->AddTransfer(transfer);

  this->socket_->Transmit(service_data.Value().GetIp(),
                          service_data.Value().GetPort(), req_payload);
  const auto res = trans->GetACK();
  if (res.HasValue()) {
    return res.Value();
  } else {
    AppLogger::Error(
        "[SOMEIPCONTROLLER] Request to :" + std::to_string(service_id) + " : " +
        std::to_string(method_id) + " Timeout");
  }
}

simba::core::ErrorCode SomeIpController::AddMethod(const uint16_t method_id,
                                                   SomeIPMethod callback) {
  this->methods.insert({method_id, callback});
  return simba::core::ErrorCode::kOk;
}

simba::core::ErrorCode SomeIpController::AddEventValue(
    const uint16_t event_id, const std::vector<uint8_t> payload) {
  return simba::core::ErrorCode::kNotDefine;
}

simba::core::ErrorCode SomeIpController::Init() {
  AppLogger::Info("[SOMEIPCONTROLLER] Socket starting");
  /// TODO: Do this better with config or config
  if (this->socket_->Init(this->config_) == simba::core::ErrorCode::kOk) { 
    this->socket_->StartRXThread();
    AppLogger::Info("[SOMEIPCONTROLLER] Socket started RX Thhread");
  }
  return simba::core::ErrorCode::kOk;
}

void SomeIpController::RxCallback(const std::string& ip,
                                  const std::uint16_t& port,
                                  std::vector<std::uint8_t> payload) {
  const auto header = msg_factory.GetHeader(payload);
  AppLogger::Debug("[SOMEIPCONTROLLER] New data for: " +
                   std::to_string(header->GetServiceID()) +
                   " from: " + std::to_string(header->GetClientID()) +
                   " transfer: " + std::to_string(header->GetSessionID()));
  AppLogger::Info("[SOMEIPCONTROLLER] New Data");
  if (header->GetClientID() == this->service_id_) {
    this->Response(header, msg_factory.GetPayload(payload));
  } else if (header->GetServiceID() == this->service_id_) {
    this->MethodCalled(header, msg_factory.GetPayload(payload));
  } else {
    // TODO(any) : Implementacja Error nie ten serwis
  }
}

SomeIpController::SomeIpController(const uint16_t service_id,
                                   std::unique_ptr<soc::ISocket> socket,
                                   const soc::SocketConfig& config)
    : socket_{std::move(socket)}, service_id_{service_id}, config_{config} {
  socket_->SetRXCallback([this](const std::string& ip,
                                const std::uint16_t& port,
                                std::vector<std::uint8_t> payload) {
    this->RxCallback(ip, port, payload);
  });
}

simba::core::ErrorCode SomeIpController::LoadServiceList(
    const std::string& path) {
  database::json::DatabaseJsonParser::LoadJson(this->db_, path);
  AppLogger::Info("[SOMEIPCONTROLLER] loaded config file with services");
  return simba::core::ErrorCode::kOk;
}

void SomeIpController::MethodCalled(
    const std::shared_ptr<simba::com::core::someip::SomeIpHeader> header,
    std::vector<std::uint8_t> data) {
  AppLogger::Debug("[SOMEIPCONTROLLER] Call method");
  const auto obj = this->methods.find(header->GetMethodID());
  if (obj == this->methods.end()) {
    AppLogger::Error("[SOMEIPCONTROLLER] Method Not found method_id: " +
                     std::to_string(header->GetMethodID()));
    // TODO(any) : implemnet error method uknown
    return;
  }

  const auto res = obj->second(data);

  if (!res.HasValue()) {
    AppLogger::Info("[SOMEIPCONTROLLER] Socket Send Respons No Value");
    // TODO(any) : implemnet error kNOK
  }

  if (res.Value().second != com::core::data::MessageCode::kEOk) {
    AppLogger::Info("[SOMEIPCONTROLLER] Socket Send Respons No OK");
    // TODO(any) : implemnet error
  } else {
    if (header->GetMessageType() == com::core::data::MessageType::kRequest) {
      header->SetMessageType(core::data::MessageType::kResponse);
      header->SetReturnCode(core::data::MessageCode::kEOk);
      this->SendResponse(header, res.Value().first);
      AppLogger::Info("[SOMEIPCONTROLLER] Socket Send Respons");
    }
  }
}
void SomeIpController::Response(
    const std::shared_ptr<simba::com::core::someip::SomeIpHeader> header,
    std::vector<std::uint8_t> data) {
  auto obj = std::find_if(
      this->transfers.begin(), this->transfers.end(),
      [header](std::shared_ptr<simba::com::someip::data::Transfer> tr) {
        return tr->GetTransferID() == header->GetSessionID();
      });
  if (obj == this->transfers.end()) {
    return;
  }
  AppLogger::Info("Response data size: " + std::to_string(data.size()));
  obj->get()->SetResponsed(data);
  AppLogger::Info("Response done!");
}
void SomeIpController::UknowReqeust(
    const std::shared_ptr<simba::com::core::someip::SomeIpHeader> header,
    std::vector<std::uint8_t> data) {}
void SomeIpController::SendResponse(
    const std::shared_ptr<simba::com::core::someip::SomeIpHeader> header,
    std::vector<std::uint8_t> data) {
  const auto service_id = header->GetClientID();
  const auto service_data = db_.GetService(service_id);
  if (!service_data.HasValue()) {
    AppLogger::Error("[SOMEIPCONTROLLER] (" + std::string(__func__) +
                     ") Service_id: " + std::to_string(service_id) +
                     " not found!");
  }

  this->socket_->Transmit(service_data.Value().GetIp(),
                          service_data.Value().GetPort(),
                          msg_factory.GetBuffor(header, header->GetClientID(),
                                                header->GetSessionID(), data));
}
}  // namespace someip
}  // namespace com
}  // namespace simba
