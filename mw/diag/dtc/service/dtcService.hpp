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
#include "diag/dtc/factories/dtc_msg_factory.hpp"
#include "diag/base/data/data_structure.h"
#include "diag/base/data/parser.h"
#include "diag/base/factories/diag_data_factory.h"

#include <fstream>
#include <memory>
#include <string>
#include <bitset>

namespace simba{
namespace mw{
namespace dtc{


class Converter{
  public:
   uint16_t convertVecToUint16(std::vector<uint8_t> data,uint8_t start_index=0){
      uint16_t d=(static_cast<std::uint16_t>(data[start_index]) << 8) |
      static_cast<std::uint16_t>(data[start_index+1]) |
      (static_cast<std::uint16_t>(data[start_index+2]) << 8) |
      static_cast<std::uint16_t>(data[start_index+3]);
      return d;
   }
    std::string convertVecToString(std::vector<uint8_t> data,uint8_t start_index){
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
};

class DtcService:public core::ApplicationNoIPC{
 protected:
   com::soc::IpcSocket dtc_sock_{};
   com::soc::IpcSocket diag_sock_{};
   DtcDatabase db_{};
   Converter conv_{};
   diag::Parser diag_parser_{};
   diag::DiagDataFactory diag_factory_{};
   uint32_t transfer_id=0;


 public:
   DtcService(){
   }
   /**
    * @brief callback for diag.dtc communication
    * 
    * @param ip 
    * @param port 
    * @param data 
    */
    void DiagRxCallback(const std::string& ip, const std::uint16_t& port,const std::vector<std::uint8_t> data) {
      auto hdr=this->diag_parser_.GetStructure(data);
      if (!hdr.HasValue()){
        AppLogger::Warning("Brak danych w DiagRXCallback");
      }
      std::bitset<16> diagId{hdr.Value().GetDiagID()};
      std::bitset<4> status(diagId.to_ulong()>>4);
      switch(status.to_ulong()){
        case 0x0:
         //TODO WRITE METHODS
        break;
        case 0x1:
         //TODO READ METHODS
        break;
        case 0x2:
        //TODO JOB METHODS
        break;
        case 0x3:
         //TODO RESPONSE IGNORE??
        break;
      }
    }
    /**
     * @brief Callback for dtc interface
     * 
     * @param ip 
     * @param port 
     * @param data 
     */
    void DtcRxCallback(const std::string& ip, const std::uint16_t& port,const std::vector<std::uint8_t> data) {
        diag::dtc::DtcMsgFactory factory;
        auto hdr=factory.GetHeader(data);
        std::vector<uint8_t> payload=factory.GetPayload(data);
        this->db_.AddError(hdr->GetDtcID(),this->conv_.convertVecToString(payload,0));
        AppLogger::Debug("Zarejestrowano błąd "+std::to_string(static_cast<int>(hdr->GetDtcID())));
        //TOFIX poprawa generowania ramek, i wysylanie na diag interfejs informacji o błędzie
        this->diag_factory_.CreatError(this->data["dtc"],this->data["dtc-diag"],this->data["dtc"],this->transfer_id,{},hdr->GetDtcID());
    }
    void Run(const std::unordered_map<std::string, core::Parm>& parms) {
        std::ifstream f("mw/diag/dtc/config.json");
        if(f.is_open()){
        this->data=nlohmann::json::parse(f);
        }else{
          AppLogger::Error("File not found");
          exit(1);
        }
        this->diag_sock_.Init(com::soc::SocketConfig(this->data["diag"],0,0));
        this->dtc_sock_.Init(com::soc::SocketConfig(this->data["dtc"], 0, 0));
        this->diag_sock_.SetRXCallback(std::bind(&DtcService::DiagRxCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->dtc_sock_.SetRXCallback(std::bind(&DtcService::DtcRxCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->diag_sock_.StartRXThread();
        this->dtc_sock_.StartRXThread();
        while(true){
          this->dtc_sock_.Transmit("0x0101.dtc",0,{0,1,2,3,4,5,6});
          this->diag_sock_.Transmit("0x0101.diag",0,{0,1,2,3,4,5,6});
        }
    }
  
};

} // dtc
} // mw
} // simba


#endif // _MW_DTC_SERVICE_HPP_