/**
 * @file dtcService.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _MW_DTC_SERVICE_HPP_
#define _MW_DTC_SERVICE_HPP_

#include "mw/diag/dtc/IDtcService.hpp"
#include "mw/diag/dtc/database/dtc_database.hpp"
#include "mw/diag/dtc/database/dtc_database_element.hpp"
#include "communication-core/sockets/ipc_socket.h"
#include "communication-core/sockets/socket_config.h"
#include "core/application/application_no_ipc.h"
#include "core/application/application_factory.h"
#include "core/logger/ILogger.h"

namespace simba{
namespace mw{
namespace dtc{


class DtcService:public core::ApplicationNoIPC{
 protected:
   com::soc::IpcSocket sock_{};
   DtcDatabase db_{};

 public:
   DtcService(){
   }
    void RxCallback(const std::string& ip, const std::uint16_t& port, std::vector<std::uint8_t> data) {
        AppLogger::Debug("Received data");
        std::cout << "Received data size: " << sizeof(this->data) << std::endl;
    }

    void Run(const std::unordered_map<std::string, core::Parm>& parms) {
        this->sock_.Init(com::soc::SocketConfig("simba/engine/dtc", 0, 0));
        this->sock_.SetRXCallback(std::bind(&DtcService::RxCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->sock_.StartRXThread();

        while (true) {
          if(this->sock_.Transmit("simba/engine/dtc", 0,std::vector<uint8_t>{0,1,2,3})!=core::ErrorCode::kOk){
            AppLogger::Warning("error");
          }
          
            std::this_thread::sleep_for(std::chrono::seconds{2});
            AppLogger::Debug("wtf");
        }
    }
  
};

} // dtc
} // mw
} // simba


#endif // _MW_DTC_SERVICE_HPP_