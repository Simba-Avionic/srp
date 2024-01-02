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

#include "mw/diag/dtc/service/IDtcService.hpp"
#include "mw/diag/dtc/database/dtc_database.hpp"
#include "mw/diag/dtc/database/dtc_database_element.hpp"
#include "communication-core/sockets/ipc_socket.h"
#include "communication-core/sockets/socket_config.h"
#include "core/application/application_no_ipc.h"
#include "core/application/application_factory.h"
#include "core/logger/ILogger.h"
#include "mw/diag/dtc/msg/dtc_msg_factory.hpp"

#include <fstream>
#include <memory>
#include <string>

namespace simba{
namespace mw{
namespace dtc{


class DtcService:public core::ApplicationNoIPC{
 protected:
   com::soc::IpcSocket sock_{};
   DtcDatabase db_{};

 public:
   DtcService(){

    std::ifstream f("mw/diag/dtc/config.json");
    this->data=nlohmann::json::parse(f);
   }
    uint16_t convertTouint16(std::vector<uint8_t> data,uint8_t start_index=0){
      uint16_t d=(static_cast<std::uint16_t>(data[start_index]) << 8) |
        static_cast<std::uint16_t>(data[start_index+1]) |
        (static_cast<std::uint16_t>(data[start_index+2]) << 8) |
        static_cast<std::uint16_t>(data[start_index+3]);
        return d;
    }

    std::string convertToString(std::vector<uint8_t> data,uint8_t start_index){
      return std::string(data.begin() + start_index, data.end());
    }

    std::vector<uint8_t> convertUint16ToVector(uint16_t value) {
      std::vector<uint8_t> result;

      result.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
      result.push_back(static_cast<uint8_t>(value & 0xFF));

      return result;
  }
  std::vector<uint8_t> convertStringToVector(const std::string& str) {
    std::vector<uint8_t> result;

    for (char ch : str) {
        result.push_back(static_cast<uint8_t>(ch));
    }

    return result;
}


    void RxCallback(const std::string& ip, const std::uint16_t& port,const std::vector<std::uint8_t> data) {
        DtcMsgFactory factory;
        auto hdr=factory.GetHeader(data);
        std::vector<uint8_t> payload=factory.GetPayload(data);
        std::string error_details="";
        uint16_t error_id=0;
        static uint16_t error_num=0;
        switch(hdr->GetMethodID()){
          case 0x7F00:
              error_id=this->db_.AddError(hdr->GetServiceID(),this->convertTouint16(payload,0),
                                this->convertToString(payload,4));
              AppLogger::Info("Registered error with id:"+std::to_string(static_cast<int>(error_id)));
              this->sock_.Transmit(std::to_string(static_cast<int>(hdr->GetServiceID()))
                            ,0,this->convertUint16ToVector(error_id));
               AppLogger::Info("Transfer error id to client");              
            break;
          case 0x7F01:
              error_details=this->db_.GetErrorDetails(this->convertTouint16(payload));
              AppLogger::Info("Requested details of error with id:"+std::to_string(static_cast<int>(error_id)));
              this->sock_.Transmit(std::to_string(static_cast<int>(hdr->GetServiceID()))
                            ,0,this->convertStringToVector(error_details));
              AppLogger::Info("Return Details:"+error_details);
            break;
          case 0x7F02:
              error_num=this->db_.ErrorNum();
              AppLogger::Info("Requested error number: "+std::to_string(static_cast<int>(error_num)));
              this->sock_.Transmit(std::to_string(static_cast<int>(hdr->GetServiceID()))
                            ,0,this->convertUint16ToVector(error_num));
              AppLogger::Info("Response for Request error number");
            break;
          case 0x7F03:
              error_id=this->db_.ManResetError(this->convertTouint16(payload));
              AppLogger::Warning("Manual reset Error with id:"+std::to_string(static_cast<int>(error_id)));
              this->sock_.Transmit(std::to_string(static_cast<int>(hdr->GetServiceID()))
                            ,0,this->convertUint16ToVector(error_id));
            break;
          case 0x7F04:
              error_id=this->db_.AutoResetError(this->convertTouint16(payload));
              AppLogger::Warning("Auto reset Error with id:"+std::to_string(static_cast<int>(error_id)));
              this->sock_.Transmit(std::to_string(static_cast<int>(hdr->GetServiceID()))
                            ,0,this->convertUint16ToVector(error_id));
            break;
        }

    }

    void Run(const std::unordered_map<std::string, core::Parm>& parms) {
        this->sock_.Init(com::soc::SocketConfig(this->data["dtc-service"]["ipc"], 0, 0));
        this->sock_.SetRXCallback(std::bind(&DtcService::RxCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->sock_.StartRXThread();
        DtcMsgFactory factory;
        while (true) {
          std::this_thread::sleep_for(std::chrono::seconds(2));
          uint16_t service_id = 0x1234;
          uint16_t method_id = 0x5678;
          std::shared_ptr<DtcHeader> hdr = std::make_shared<DtcHeader>(service_id, method_id);
          std::vector<uint8_t> payload={0,1,2,3,4};
          std::vector<uint8_t> toSend=factory.GetBuffer(hdr,payload);
          this->sock_.Transmit(this->data["dtc-service"]["ipc"],0,toSend);
        }
    }
  
};

} // dtc
} // mw
} // simba


#endif // _MW_DTC_SERVICE_HPP_