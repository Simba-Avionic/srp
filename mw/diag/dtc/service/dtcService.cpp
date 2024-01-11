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

#include <vector>
#include <unordered_map>

#include "mw/diag/dtc/service/dtcService.hpp"

namespace simba {
namespace mw {
namespace dtc {

namespace {
static constexpr uint16_t dtc_id = 0x101;
}


DtcService::DtcService() {
  this->app_name = "DTC-Service";
  this->diag_controller_ = std::make_unique<diag::DiagController>(
      dtc_id, std::make_unique<com::soc::IpcSocket>());
  this->diag_controller_->Init();
  // TODO(matikrajek42@gmail.com) dodanie obsługi metod diagnostycznych
  // this->diag_controller_->AddMethod()
}
/**
 * @brief Callback for dtc interface
 * 
 * @param ip 
 * @param port 
 * @param data 
 */
void DtcService::DtcRxCallback(const std::string& ip,
  const std::uint16_t& port, const std::vector<std::uint8_t> data) {
  diag::dtc::DtcMsgFactory factory;
  auto hdr = factory.GetHeader(data);
  std::vector<uint8_t> payload = factory.GetPayload(data);
  this->db_.AddError(hdr->GetDtcID(),
  this->conv_.convertVecToString(payload, 0));
  AppLogger::Debug("Zarejestrowano błąd "
  +std::to_string(static_cast<int>(hdr->GetDtcID())));
  // TODO(matikrajek42@gmail.com) dodaj wysylanie informacji o wystąpieniu błędu
  this->diag_controller_->Write(0x0201, 0x0001,
        this->conv_.convertUint16ToVector(hdr->GetDtcID()));
}

void DtcService::Run(const std::unordered_map<std::string,
                                     core::Parm>& parms) {
  this->dtc_sock_.Init(com::soc::SocketConfig("SIMBA.DIAG.DTC."
                        +this->conv_.uint16ToString(dtc_id), 0, 0));
  this->dtc_sock_.SetRXCallback(std::bind(&DtcService::DtcRxCallback,
  this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
  this->dtc_sock_.StartRXThread();
  std::this_thread::sleep_for(std::chrono::hours::max());
  }


}  // namespace dtc
}  // namespace mw
}  // namespace simba
