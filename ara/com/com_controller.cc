/**
 * @file com_controller.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/com/com_controller.h"

#include <memory>
#include <utility>

#include "ara/com/com_error_domain.h"
#include "ara/com/log.h"
#include "ara/log/logging_menager.h"
namespace ara {
namespace com {
namespace {
static std::shared_ptr<ComController> instance_;
}  // namespace

ComController::ComController(const uint32_t& app_id) : app_id_{app_id} {}

ComController::~ComController() {}

ara::core::Result<void> ComController::Init() noexcept {
  if (ipc_soc_ != nullptr) {
    return MakeErrorCode(ComOfferErrc::kAlreadyOffered,
                         "Controller already Started");
  }
  ipc_soc_ = std::make_unique<ProccessSocket>(app_id_);

  ipc_soc_->SetCallback(std::bind(&ComController::IpcRxCallback, this,
                                  std::placeholders::_1,
                                  std::placeholders::_2));

  const auto res = ipc_soc_->Offer();
  if (!res.HasValue()) {
    ara::com::LogFatal() << res.Error().Message() << ": "
                         << res.Error().SupportData();
  }
  return {};
}
void ComController::IpcRxCallback(const uint32_t pid,
                                  const std::vector<uint8_t>& payload) {
  LogDebug() << "New msg from: " << pid << " with payload: " << payload;
  const auto& item =
      handlers_.find(static_cast<IComClient::MsgType>(payload[0]));
  if (item != handlers_.end()) {
    item->second->HandleNewMsg(
        pid, std::vector<uint8_t>(payload.begin() + 1, payload.end()));
  } else {
    LogError() << "New msg for not supported handler type: " << payload[0];
  }
}

bool ComController::AddHandler(IComClient::MsgType msg_type,
                               std::shared_ptr<IComClient> handler_) {
  LogDebug() << "New Handler for: " << msg_type << " added";
  handler_->SetSendCallback(
      std::bind(&ComController::SendCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3),
      std::bind(&ComController::SendCallbackTo, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

  return handlers_.insert({msg_type, handler_}).second;
}
bool ComController::SendCallback(uint32_t pid,
                                 const std::vector<uint8_t>& payload,
                                 IComClient::MsgType type) {
  std::vector<uint8_t> temp_payload{type};
  temp_payload.insert(temp_payload.end(), payload.begin(), payload.end());
  return this->ipc_soc_->TransmitToPid(pid, std::move(temp_payload)).HasValue();
}

bool ComController::SendCallbackTo(const std::string& desc,
                                   const std::vector<uint8_t>& payload,
                                   IComClient::MsgType type) {
  LogDebug() << "Sending to: " << desc;
  std::vector<uint8_t> temp_payload{type};
  temp_payload.insert(temp_payload.end(), payload.begin(), payload.end());
  if (ipc_soc_ != nullptr) {
    return this->ipc_soc_->Transmit(desc, temp_payload).HasValue();
  } else {
    LogError() << "Missing ipc sock!!";
    return false;
  }
}

ComController& ComController::GetInstance() noexcept { return *instance_; }
ComController& ComController::GetInstance(const uint32_t app_id) noexcept {
  if (nullptr == instance_) {
    instance_ = std::make_unique<ComController>(app_id);
  }
  return *instance_;
}
}  // namespace com
}  // namespace ara
