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
#include "someip-controller/someip_controller.h"
#include "json-parser/database_json_parser.h"
#include "results/result.h"
#include "someip/message_code.h"
#include "someip/message_type.h"
#include "someip/someip_header.h"
#include "database/service_element.h"
#include <string>
namespace simba {
namespace com {
namespace someip {
simba::core::Result<std::vector<uint8_t>> SomeIpController::Request(
    const uint16_t service_id, const uint16_t method_id,
    const std::vector<uint8_t> payload) {
  const auto service_data = db_.GetService(service_id);
  if (!service_data.HasValue()) {
    this->logger_->Error("[SOMEIPCONTROLLER] Service_id: " +
                         std::to_string(service_id) + " not found!");
    return simba::core::Result<std::vector<uint8_t>>{};
  } else {
    this->logger_->Error(
        "[SOMEIPCONTROLLER] Service_id: " + std::to_string(service_id) +
        " ip = " + service_data.Value().GetIp());
  }
  const auto transfer = GetTransferID();
  auto req = header_factory.CreateRequest(service_id, method_id);
  auto req_payload = msg_factory.GetBuffor(req, service_id_, transfer, payload);
  auto trans = this->AddTransfer(transfer);
  logger_->Debug("Sending to: " + std::to_string(req->GetServiceID()) +
                 " From " + std::to_string(req->GetClientID()));
  if (this->socket_->Transmit(service_data.Value().GetIp(),
                              service_data.Value().GetPort(),
                              req_payload) != simba::core::ErrorCode::kOk) {
    std::remove_if(
        this->transfers.begin(), transfers.end(),
        [trans](std::shared_ptr<simba::com::someip::data::Transfer> tr) {
          return tr->GetTransferID() == trans->GetTransferID();
        });
    logger_->Error(
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
    logger_->Error(
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
    this->logger_->Error("[SOMEIPCONTROLLER] Service_id: " +
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
    logger_->Error(
        "[SOMEIPCONTROLLER] Request to :" + std::to_string(service_id) + " : " +
        std::to_string(method_id) + " Timeout");
  }
}

simba::core::ErrorCode SomeIpController::AddMethod(const uint16_t method_id,
                                                   SomeIPMethod callback) {
  auto data == this->methods.find(method_id);
  if (data == methods.end()){
    this->methods.insert({method_id, callback});
    return simba::core::ErrorCode::kOk;
  }                                                 
  return simba::core::ErrorCode::kError;
}


/**
 * @brief Dodajemy callback dla otrzymania subskrybowanego eventu
 * 
 * @param id (service_id+event_id)
 * @param callback 
 * @return simba::core::ErrorCode 
 */
simba::core::ErrorCode AddEventCallback(
            const uint16_t service_id, const uint16_t event_id, SomeIPEvent callback) {
  auto data = this->events.find(static_cast<uint32_t>(service_id) << 16 | static_cast<uint32_t>(event_id));
  if (data == events.end()) {
    this->events.insert({id, callback})
    return simba::core::ErrorCode:kOk;
  }
  return simba::core::ErrorCode::kError;
}


/**
 * @brief Funkcja do nadawania eventow do wszystkich zainteresowanych
 * 
 * @param event_id 
 * @param payload 
 * @return simba::core::ErrorCode 
 */
simba::core::ErrorCode SomeIpController::SendEvent(
    const uint16_t event_id, const std::vector<uint8_t>& payload) {
    const auto event_data = this->event_db.find(event_id);
    if (!event_data.HasValue()) {
          this->logger_->Error("[SOMEIPCONTROLLER] Event_id: " +
                         std::to_string(event_id) + " not found!");
    return simba::core::ErrorCode::kError;
    }
    auto event = header_factory.CreateEvent(this->service_id_, event_id);
    for (ServiceElement client : event_data.Value().GetLists()) {
      const auto transfer = GetTransferID();
      auto trans = this->AddTransfer(transfer);
      auto req_payload =
      msg_factory.GetBuffor(event, client.GetServiceId(), transfer, payload);
      this->socket_->Transmit(client.GetIpAddress(),
                              client.GetPort(), req_payload);
    }
  return simba::core::ErrorCode::kOk;
}

simba::core::ErrorCode SomeIpController::Init() {
  this->db_.AddService(0x100,
                       simba::database::DatabaseElement{"testowy.socket", 0});
  this->db_.AddService(0x101,
                       simba::database::DatabaseElement{"testowy.socket2", 0});
  /// TODO: Do this better with config or config
  if (this->socket_->Init(this->config_) == simba::core::ErrorCode::kOk) {
    this->socket_->StartRXThread();
    logger_->Info("[SOMEIPCONTROLLER] Socket started RX Thhread");
  }
  return simba::core::ErrorCode::kOk;
}

void SomeIpController::RxCallback(const std::string& ip,
                                  const std::uint16_t& port,
                                  std::vector<std::uint8_t> payload) {
  const auto header = msg_factory.GetHeader(payload);
  this->logger_->Debug("[SOMEIPCONTROLLER] New data for: " +
                       std::to_string(header->GetServiceID()) +
                       " from: " + std::to_string(header->GetClientID()) +
                       " transfer: " + std::to_string(header->GetSessionID()));
  logger_->Info("[SOMEIPCONTROLLER] New Data");
  // odpowiedz na Request
  if (header->GetClientID() == this->service_id_) {
    this->Response(header, msg_factory.GetPayload(payload));
  //jesli to event
  } else if (header->GetMessageType() == 0x02) {
    // TODO(matikrajek42@gmail.com): Implementacja obsugi przychodzacego eventu
    this->EventCalled(header,msg_factory.GetPayload(payload));

  //jesli metoda
  } else if (header->GetServiceID() == this->service_id_) {
    this->MethodCalled(header, msg_factory.GetPayload(payload));
  } else {
    // TODO: Implementacja Error nie ten serwis
  }
}

SomeIpController::SomeIpController(
    const uint16_t service_id, std::unique_ptr<soc::ISocket> socket,
    std::shared_ptr<simba::core::logger::ILogger> logger,
    const soc::SocketConfig& config)
    : socket_{std::move(socket)},
      service_id_{service_id},
      logger_{logger},
      config_{config} {
  socket_->SetRXCallback([this](const std::string& ip,
                                const std::uint16_t& port,
                                std::vector<std::uint8_t> payload) {
    this->RxCallback(ip, port, payload);
  });
}

simba::core::ErrorCode SomeIpController::LoadServiceList(
    const std::string& path) {
  database::json::DatabaseJsonParser::LoadJson(this->db_, path);
  logger_->Info("[SOMEIPCONTROLLER] loaded config file with services");
  return simba::core::ErrorCode::kOk;
}

/**
 * @brief Funkcja wywoływana kiedy RxCallback wychwyci msg_type==Notification
 * 
 * @param header 
 * @param data 
 */
void SomeIpController::EventCalled(
    const std::shared_ptr<simba::com::core::someip::SomeIpHeader> header,
    std::vector<std::uint8_t> data) {
  logger_->Debug("[SOMEIPCONTROLLER] Call event");
  const auto obj = this->events.find(std::make_pair
                (header->GetServiceID(), header->GetMethodID()));
  if (obj == this->events.end()) {
    logger_->Error("[SOMEIPCONTROLLER] Event Not found event_id: " +
                   std::to_string(header->GetMethodID()));
    return
  }
  obj->second(data);
}

void SomeIpController::MethodCalled(
    const std::shared_ptr<simba::com::core::someip::SomeIpHeader> header,
    std::vector<std::uint8_t> data) {
  logger_->Debug("[SOMEIPCONTROLLER] Call method");
  const auto obj = this->methods.find(header->GetMethodID());
  if (obj == this->methods.end()) {
    logger_->Error("[SOMEIPCONTROLLER] Method Not found method_id: " +
                   std::to_string(header->GetMethodID()));
    //TODO() : implemnet error method uknown
    return;
  }
  const auto res = obj->second(data);

  if (!res.HasValue()) {
    logger_->Info("[SOMEIPCONTROLLER] Socket Send Respons No Value");
    // TODO: implemnet error kNOK
  }

  if (res.Value().second != com::core::data::MessageCode::kEOk) {
    logger_->Info("[SOMEIPCONTROLLER] Socket Send Respons No OK");
    // TODO: implemnet error
  } else {
    if (header->GetMessageType() == com::core::data::MessageType::kRequest) {
      header->SetMessageType(core::data::MessageType::kResponse);
      header->SetReturnCode(core::data::MessageCode::kEOk);
      this->SendResponse(header, res.Value().first);
      logger_->Info("[SOMEIPCONTROLLER] Socket Send Respons");
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
  logger_->Info("Response data size: " + std::to_string(data.size()));
  obj->get()->SetResponsed(data);
  logger_->Info("Response done!");
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
    this->logger_->Error("[SOMEIPCONTROLLER] (" + std::string{__func__} +
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
