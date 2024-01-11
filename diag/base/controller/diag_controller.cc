/**
 * @file diag_controller.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-03
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "diag/base/controller/diag_controller.h"

#include <functional>
#include <string>
#include <utility>

#include "communication-core/sockets/socket_config.h"
#include "core/common/error_code.h"
#include "core/logger/Logger.h"
#include "core/results/result.h"
#include "diag/base/data/data_structure.h"
#include "diag/base/data/parser.h"
#include "diag/base/factories/diag_data_factory.h"
namespace simba {
namespace diag {

simba::core::ErrorCode DiagController::AddMethod(
    const uint8_t diag_id, DiagMethod callback,
    const DiagMethodType method_type) {
  if (diag_id > 0x3F) {
    AppLogger::Error("[DIAG_CONTROLLER] bad diag_id. Diag_id > 0x3F");
    return core::ErrorCode::kBadVariableSize;
  }
  const uint8_t diag_id_number =
      ((diag_id << 0x02) | static_cast<std::uint8_t>(method_type));
  if (this->callback_list.find(diag_id_number) != this->callback_list.end()) {
    AppLogger::Error(
        "[DIAG_CONTROLLER] Diag_id: " + std::to_string(diag_id) +
        " already registered with flag: " + std::to_string(method_type));
    return core::ErrorCode::kError;
  }
  AppLogger::Info(
      "[DIAG_CONTROLLER] Diag_id added: " + std::to_string(diag_id) +
      " with flag: " + std::to_string(method_type) + " = " +
      std::to_string(diag_id_number));
  this->callback_list.insert({diag_id_number, callback});
  return core::ErrorCode::kOk;
}

simba::core::ErrorCode DiagController::Init() {
  if (this->socket_driver == nullptr) {
    return core::ErrorCode::kError;
  }

  this->socket_driver->SetRXCallback(
      std::bind(&DiagController::RxCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
  if (this->socket_driver->Init(com::soc::SocketConfig{
          "SIMBA.DIAG." + std::to_string(this->service_id_), 0, 0}) ==
      core::ErrorCode::kOk) {
    this->socket_driver->StartRXThread();
    AppLogger::Info("[DIAG_CONTROLLER] IPC socket initialized");
    return core::ErrorCode::kOk;
  }
  AppLogger::Error("[DIAG_CONTROLLER] Can't initialized ipc socket");
  return core::ErrorCode::kError;
}

void DiagController::RxCallback(const std::string& ip,
                                const std::uint16_t& port,
                                std::vector<std::uint8_t> payload) {
  const auto data_r = diag::Parser::GetStructure(payload);
  if (!data_r.HasValue()) {
    return;
  }
  const auto data = data_r.Value();
  if (data.GetServiceID() != this->service_id_) {
    return;
  }

  if (data.isResponse()) {
    this->Response(data);
  } else {
    if (this->callback_list.find(data.GetDiagID()) ==
        this->callback_list.end()) {
      AppLogger::Warning("[DIAG_CONTROLLER] Diag event not found (" +
                         std::to_string(data.GetDiagID() >> 0x02) + ")");
      this->SendError(data);
      return;
    }
    AppLogger::Info("[DIAG_CONTROLLER] Diag event received (" +
                    std::to_string(data.GetDiagID() >> 0x02) + ") -> " +
                    std::to_string(data.GetDiagID()));
    this->Request(data);
  }
}

void DiagController::Request(const data::DataStructure req) {
  auto obj = this->callback_list.find(req.GetDiagID());
  if (obj == this->callback_list.end()) {
    AppLogger::Error("[DIAG_CONTROLLER] Diag id unexpected not founded: ");
    return;
  }

  const auto result = obj->second(req.GetPayload());
  if (result.HasValue()) {
    const auto res_t = DiagDataFactory::CreateResponse(
        req.GetSenderID(), req.GetDiagID() >> 0x2, this->service_id_,
        req.GetTransferID(), result.Value());
    if (!res_t.HasValue()) {
      AppLogger::Error(
          "[DIAG_CONTROLLER] An error occurred while creating the answer");
      return;
    }
    if (this->Send(res_t.Value()) != core::ErrorCode::kOk) {
      AppLogger::Error(
          "[DIAG_CONTROLLER] Cannot send response to: SIMBA.DIAG." +
          std::to_string(req.GetSenderID()));
    }
  } else {
    this->SendError(req);
  }
}

simba::core::ErrorCode DiagController::Send(const data::DataStructure& req) {
  const auto buffer_t = Parser::GetBuffer(req);
  if (!buffer_t.HasValue()) {
    AppLogger::Error("[DIAG_CONTROLLER] An error occurred while parsing");
    return core::ErrorCode::kError;
  }
  const auto buffer = buffer_t.Value();
  if (this->socket_driver != nullptr) {
    return this->socket_driver->Transmit(
        "SIMBA.DIAG." + std::to_string(req.GetServiceID()), 0, buffer);
  }
  return core::ErrorCode::kError;
}

void DiagController::Response(const data::DataStructure req) {
  auto trans =
      std::find_if(this->transfer_list.begin(), this->transfer_list.end(),
                   [req](const std::shared_ptr<DiagTransfer> t) {
                     return t->GetTransferID() == req.GetTransferID();
                   });
  if (trans == this->transfer_list.end()) {
    AppLogger::Warning("[DIAG_CONTROLLER] Unexpected transfer");
    return;
  }
  if (req.GetDiagID() >> 0x2 == 0x3F) {
    trans->get()->NegativeResponse();
  } else {
    trans->get()->SetResponse(req.GetPayload());
  }
}

void DiagController::SendError(const data::DataStructure req) {
  const auto res_t = diag::DiagDataFactory::CreateResponse(
      req.GetSenderID(), 0x3F, this->service_id_, req.GetTransferID(), {});
  if (!res_t.HasValue()) {
    AppLogger::Error(
        "[DIAG_CONTROLLER] An error occurred while creating the answer");
    return;
  }
  const auto res = res_t.Value();
  if (this->Send(res) != core::ErrorCode::kOk) {
    AppLogger::Error(
        "[DIAG_CONTROLLER] Cannot send error response to: SIMBA.DIAG." +
        std::to_string(req.GetServiceID()));
  }
}

simba::core::Result<std::vector<uint8_t>> DiagController::Read(
    const uint16_t service_id, const uint8_t diag_id) {
  const auto t_id = this->GetTransferId();
  const auto req_t = DiagDataFactory::CreateReadRequest(
      service_id, diag_id, this->service_id_, t_id, {});
  if (!req_t.HasValue()) {
    AppLogger::Error(
        "[DIAG_CONTROLLER] An error occurred while creating the Read request");
    return simba::core::Result<std::vector<uint8_t>>{};
  }
  auto transfer = std::make_shared<DiagTransfer>(t_id);
  if (this->Send(req_t.Value()) != core::ErrorCode::kOk) {
    AppLogger::Error(
        "[DIAG_CONTROLLER] Cannot send Read request to: SIMBA.DIAG." +
        std::to_string(req_t.Value().GetServiceID()));
    return simba::core::Result<std::vector<uint8_t>>{};
  }
  this->transfer_list.push_back(transfer);
  const auto result = transfer->GetPayloadAsc();
  std::remove_if(this->transfer_list.begin(), this->transfer_list.end(),
                 [transfer](std::shared_ptr<DiagTransfer> t) {
                   return t->GetTransferID() == transfer->GetTransferID();
                 });
  return result;
}

simba::core::ErrorCode DiagController::Write(
    const uint16_t service_id, const uint8_t diag_id,
    const std::vector<uint8_t> payload) {
  const auto t_id = this->GetTransferId();
  const auto req_t = DiagDataFactory::CreateWriteRequest(
      service_id, diag_id, this->service_id_, t_id, payload);
  if (!req_t.HasValue()) {
    AppLogger::Error(
        "[DIAG_CONTROLLER] An error occurred while creating the Read request");
    return core::ErrorCode::kError;
  }
  auto transfer = std::make_shared<DiagTransfer>(t_id);
  if (this->Send(req_t.Value()) != core::ErrorCode::kOk) {
    AppLogger::Error(
        "[DIAG_CONTROLLER] Cannot send Write request to: SIMBA.DIAG." +
        std::to_string(req_t.Value().GetServiceID()));
    return core::ErrorCode::kConnectionError;
  }
  this->transfer_list.push_back(transfer);
  const auto result = transfer->GetPayloadAsc();
  std::remove_if(this->transfer_list.begin(), this->transfer_list.end(),
                 [transfer](std::shared_ptr<DiagTransfer> t) {
                   return t->GetTransferID() == transfer->GetTransferID();
                 });
  if (result.HasValue()) {
    return core::ErrorCode::kOk;
  } else {
    return core::ErrorCode::kError;
  }
}
simba::core::Result<std::vector<uint8_t>> DiagController::Job(
    const uint16_t service_id, const uint8_t diag_id,
    const std::vector<uint8_t> payload) {
  const auto t_id = this->GetTransferId();
  const auto req_t = DiagDataFactory::CreateJobRequest(
      service_id, diag_id, this->service_id_, t_id, payload);
  if (!req_t.HasValue()) {
    AppLogger::Error(
        "[DIAG_CONTROLLER] An error occurred while creating the Read request");
    return simba::core::Result<std::vector<uint8_t>>{};
  }
  auto transfer = std::make_shared<DiagTransfer>(t_id);
  if (this->Send(req_t.Value()) != core::ErrorCode::kOk) {
    AppLogger::Error(
        "[DIAG_CONTROLLER] Cannot send Job request to: SIMBA.DIAG." +
        std::to_string(req_t.Value().GetSenderID()));
    return simba::core::Result<std::vector<uint8_t>>{};
  }
  this->transfer_list.push_back(transfer);
  const auto result = transfer->GetPayloadAsc();
  std::remove_if(this->transfer_list.begin(), this->transfer_list.end(),
                 [transfer](std::shared_ptr<DiagTransfer> t) {
                   return t->GetTransferID() == transfer->GetTransferID();
                 });
  return result;
}

DiagController::DiagController(const uint16_t service_id,
                               std::unique_ptr<com::soc::ISocket> soc)
    : service_id_{service_id}, socket_driver{std::move(soc)} {
    }

const uint16_t DiagController::GetTransferId() { return this->transfer_id++; }

}  // namespace diag
}  // namespace simba
