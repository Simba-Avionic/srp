/**
 * @file dtcService.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "mw/diag/dtc/service/dtcService.h"

#include <unordered_map>
#include <vector>

namespace simba {
namespace mw {
namespace dtc {

<<<<<<< HEAD
namespace {
static constexpr uint16_t dtc_id = 0x101;
=======
DtcService::DtcService() {}
core::ErrorCode DtcService::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  db_ = std::make_shared<DataBase>();
  dtc_list_ = std::make_unique<DtcListJob>("DtcService/DtcList", db_);
  dtc_by_mask_job_ =
      std::make_unique<DtcByMaskJob>("DtcService/DtcByMask", db_);
  dtc_snapshot_job_ =
      std::make_unique<DtcSnapshotJob>("DtcService/dtc_snapshot");
  clear_dtc_job_ =
      std::make_unique<ClearDtcJob>("DtcService/clear_memory", db_);
  this->LoadConfig("/opt/" + parms.at("app_name") +
                   "/etc/dtc_support_list.json");
  return core::ErrorCode::kOk;
>>>>>>> 66b75b3 (Ara: Adding support for ARA)
}

DtcService::DtcService() {}
/**
 * @brief Callback for dtc interface
 *
 * @param ip
 * @param port
 * @param data
 */
void DtcService::DtcRxCallback(const std::string& ip, const std::uint16_t& port,
                               const std::vector<std::uint8_t> data) {
  diag::dtc::DtcMsgFactory factory;
  auto hdr = factory.GetHeader(data);
  std::vector<uint8_t> payload = factory.GetPayload(data);
  this->db_.AddError(hdr->GetDtcID(),
                     this->conv_.convertVecToString(payload, 0));
  AppLogger::Debug("Zarejestrowano błąd " +
                   std::to_string(static_cast<int>(hdr->GetDtcID())));
  // TODO(matikrajek42@gmail.com) dodaj wysylanie informacji o wystąpieniu błędu
  this->diag_controller->Write(
      0x0201, 0x0001, this->conv_.convertUint16ToVector(hdr->GetDtcID()));
}
core::ErrorCode DtcService::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  return core::ErrorCode::kOk;
}
core::ErrorCode DtcService::Run(std::stop_token token) {
<<<<<<< HEAD
  this->dtc_sock_.Init(com::soc::SocketConfig(
      "SIMBA.DIAG.DTC", 0, 0));
  this->dtc_sock_.SetRXCallback(
      std::bind(&DtcService::DtcRxCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
  this->dtc_sock_.StartRXThread();
=======
  dtc_list_->StartService();
  dtc_by_mask_job_->StartService();
  dtc_snapshot_job_->StartService();
  clear_dtc_job_->StartService();
  AppLogger::Info("App started");
>>>>>>> 66b75b3 (Ara: Adding support for ARA)
  this->SleepMainThread();
  return core::ErrorCode::kOk;
}

}  // namespace dtc
}  // namespace mw
}  // namespace simba
