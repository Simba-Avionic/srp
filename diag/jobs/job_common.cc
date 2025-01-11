/**
 * @file job_common.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-05-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "diag/jobs/job_common.h"

#include <utility>

#include "ara/core/model/diag_model_item.h"
#include "ara/core/model_db.h"
#include "ara/log/logging_menager.h"
#include "communication-core/sockets/stream_ipc_socket.h"
namespace srp {
namespace diag {
std::vector<uint8_t> JobCommon::rx_callback(const std::string&,
                                            const std::uint16_t&,
                                            std::vector<std::uint8_t> payload) {
  if (payload.size() < 2) {
    return {0x7f, payload[0], 0x10};
  }
  const auto s_id = payload[0];
  const auto res = this->Method(DiagRequest(std::move(payload)));
  return res.ParseToVector();
}

void JobCommon::StopOffer() {
  if (this->job_id_.size() == 0) {
    return;
  }
  this->soc_->StopRXThread();
}

void JobCommon::StartOffer() {
  if (this->job_id_.size() == 0) {
    return;
  }
  this->soc_ = std::make_unique<com::soc::StreamIpcSocket>();
  this->soc_->SetRXCallback(
      std::bind(&JobCommon::rx_callback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
  if (this->soc_->Init(com::soc::SocketConfig{"SRP.DIAG." + this->job_id_, 0,
                                              0}) == core::ErrorCode::kOk) {
    this->soc_->StartRXThread();
    diag_logger.LogInfo() << "[" << instance_ << "]: Socket started";
  } else {
    diag_logger.LogError() << "[" << instance_
                           << "]: An error occurred while booting the socket";
  }
}
JobCommon::JobCommon(const ara::core::InstanceSpecifier& instance)
    : instance_{instance},
      diag_logger{
          ara::log::LoggingMenager::GetInstance()->CreateLogger("diag")} {
  const auto res =
      ara::core::ModelDataBase::GetInstance()
          .ResolveInstanceSpecifier<ara::core::model::ModelUds>(instance);
  if (res.HasValue()) {
    const auto val = res.Value();
    this->job_id_ = val.com_id_;
    diag_logger.LogInfo() << "Mapping: " << instance
                          << " to id: " << this->job_id_;
  } else {
    diag_logger.LogError() << "Can't map: " << instance;
  }
}
}  // namespace diag
}  // namespace srp
