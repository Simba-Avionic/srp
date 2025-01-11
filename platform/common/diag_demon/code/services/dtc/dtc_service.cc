/**
 * @file dtc_service.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "platform/common/diag_demon/code/services/dtc/dtc_service.h"

#include "ara/log/logging_menager.h"
#include "srp/platform/diag/DtcComDataStructure.h"

namespace srp {
namespace platform {
namespace diag_demon {
namespace dtc {
DtcService::DtcService(/* args */)
    : sock_{std::make_unique<ara::com::ProccessSocket>("SRP.ARA.DTC")},
      logger_{ara::log::LoggingMenager::GetInstance()->CreateLogger(
          "dtcs", "", ara::log::LogLevel::kDebug)} {}

void DtcService::Start() noexcept {
  logger_.LogInfo() << "DtcService started";
  sock_->SetCallback(std::bind(&DtcService::RXCallback, this,
                               std::placeholders::_1, std::placeholders::_2));
  sock_->Offer();
}
void DtcService::Stop() noexcept {
  logger_.LogInfo() << "DtcService Stoped";
  sock_->StopOffer();
}
void DtcService::RXCallback(const uint32_t peerid,
                            const std::vector<uint8_t>& pyload) {
  const auto val_o =
      srp::data::Convert<srp::platform::diag::DtcComDataStructure>::Conv(
          std::vector<uint8_t>{pyload.begin() + 1, pyload.end()});
  if (val_o.has_value()) {
    const auto& val = val_o.value();
    logger_.LogInfo() << "New dtc msg from: " << peerid
                      << ", type: " << val.type << ", dtc id: " << val.dtc_id
                      << ", parm: " << val.parm;
  } else {
    logger_.LogError() << "Invalid msg recived from: " << peerid;
  }
}
}  // namespace dtc
}  // namespace diag_demon
}  // namespace platform
}  // namespace srp
