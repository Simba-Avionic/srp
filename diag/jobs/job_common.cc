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

#include "communication-core/sockets/stream_ipc_socket.h"
#include "core/logger/Logger.h"
#include "diag/config_controller/config_controller.h"
namespace simba {
namespace diag {
std::vector<uint8_t> JobCommon::rx_callback(const std::string&,
                                            const std::uint16_t&,
                                            std::vector<std::uint8_t> payload) {
  if (payload.size() < 2) {
    return {0x7f, payload[0], 0x10};
  }
  // const auto s_id = payload[0];
  const auto res = this->Method(DiagRequest(std::move(payload)));
  return res.ParseToVector();
}

void JobCommon::StartService() {
  this->soc_ = std::make_unique<com::soc::StreamIpcSocket>();
  this->soc_->SetRXCallback(
      std::bind(&JobCommon::rx_callback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
  if (this->soc_->Init(com::soc::SocketConfig{"SIMBA.DIAG." + this->job_id_, 0,
                                              0}) == core::ErrorCode::kOk) {
    this->soc_->StartRXThread();
    AppLogger::Info("[" + instance_ + "]: Socket started");
  } else {
    AppLogger::Error("[" + instance_ +
                     "]: An error occurred while booting the socket");
  }
}
JobCommon::JobCommon(const std::string& instance) : instance_{instance} {
  const auto res = diag::config::DiagConfig::FindProvideObject(instance);
  if (res.has_value()) {
    this->job_id_ = res.value();
    AppLogger::Info("Mapping: " + instance + " to id: " + this->job_id_);
  } else {
    AppLogger::Error("Can't map: " + instance);
  }
}
}  // namespace diag
}  // namespace simba
