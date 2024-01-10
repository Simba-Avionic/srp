/**
 * @file dtc_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef DTC_CONTROLLER_DTC_DTC_CONTROLLER_HPP_
#define DTC_CONTROLLER_DTC_DTC_CONTROLLER_HPP_

#include "diag/dtc/controller/I_dtc_controller.hpp"
#include "communication-core/sockets/ipc_socket.h"
#include "diag/dtc/factories/dtc_msg_factory.hpp"


namespace simba{
namespace diag{
namespace dtc{

class DtcController:public IDtcController{
 public:
 /**
  * @brief zgłaszanie błędów do DTC service
  * 
  * @param dtc_error_id kod błędu
  * @param dtc_status flagi błędu
  * @param payload dodatkowe dane
  * @return core::ErrorCode 
  */
  core::ErrorCode RegisterError(const uint16_t &dtc_error_id,const uint8_t &dtc_status,const std::vector<uint8_t> &payload) override;
  void Init(std::shared_ptr<com::soc::SocketConfig> config) override;

 private:
    com::soc::IpcSocket sock_;
    static mw::dtc::DtcMsgFactory factory_;
};


} // dtc
} // diag
} // simba



#endif // DTC_CONTROLLER_DTC_DTC_CONTROLLER_HPP_
