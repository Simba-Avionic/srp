#include "mw/diag/dtc/msg/dtc_header.hpp"

namespace simba{
namespace mw{
namespace dtc{

DtcHeader::DtcHeader(uint16_t &service_id,uint16_t &method_id)
            :method_id_(method_id),service_id_(service_id){
                this->lenght_=0x06;
    }
DtcHeader::DtcHeader(){};

uint16_t DtcHeader::GetServiceID(){
    return this->service_id_;
};
uint16_t DtcHeader::GetMethodID(){
    return this->method_id_;
};
uint16_t DtcHeader::GetLength(){
    return this->lenght_;
};

void DtcHeader::SetServiceID(const uint16_t& value){
    this->service_id_=value;
};
void DtcHeader::SetMethodID(const uint16_t& value){
    this->method_id_=value;
};
void DtcHeader::SetLength(const uint16_t& value){
    this->lenght_=value+0x06;
};

}  // dtc
}  // mw
}  // simba
