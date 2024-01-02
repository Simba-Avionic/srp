
#include "mw/diag/dtc/client/DtcClient.hpp"
#include "mw/diag/dtc/msg/dtc_msg_factory.hpp"

#include <chrono>
#include <memory>
#include <fstream>

namespace simba{
namespace mw{
namespace dtc{

DtcClient::DtcClient(){};

void DtcClient::Init(uint16_t& service_id){
    std::fstream f("mw/diag/dtc/config.json");
    if(f.is_open()){
        this->data=nlohmann::json::parse(f);
    }else{
        AppLogger::Error("config file not open");
        return;
    }
    this->service_id=service_id;
    this->sock_.Init(com::soc::SocketConfig(std::to_string(static_cast<int>(service_id)),0,0));
    this->sock_.SetRXCallback(std::bind(&DtcClient::RxCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    this->sock_.StartRXThread();
}

void DtcClient::RxCallback(const std::string& ip, const std::uint16_t& port,const std::vector<std::uint8_t> data) {

}

void DtcClient::RegisterError(uint16_t error_code,std::string error_detail){
    //TODO
    
    DtcHeader hdr;
    hdr.SetServiceID(this->service_id);
    hdr.SetMethodID(static_cast<uint16_t>(this->data["methods"]["register"]["method_id"]));
    DtcMsgFactory factory;
    factory.GetBuffer(std::make_shared(hdr),data)
    std::vector<uint8_t> payload;
    this->sock_.Transmit(this->data["dtc-service"]["ipc"],0,payload);
}

 std::string DtcClient::GetErrorDetails(uint16_t error_id){

 }
 uint16_t DtcClient::GetNumberOfErrors(){

 }
 uint16_t DtcClient::ManualResetError(uint16_t error_id){

 }
 uint16_t DtcClient::AutoResetError(uint16_t error_id){

 }


}
}
}