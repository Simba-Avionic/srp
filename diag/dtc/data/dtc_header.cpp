#include "diag/dtc/data/dtc_header.hpp"

namespace simba{
namespace mw{
namespace dtc{

DtcHeader::DtcHeader(uint16_t &dtc_id,uint16_t &dtc_status)
            :dtc_id_(dtc_id),dtc_status_(dtc_status){
                this->lenght_=0x04;
    }
DtcHeader::DtcHeader(){};

uint16_t DtcHeader::GetDtcID(){
    return this->dtc_id_;
};
uint8_t DtcHeader::GetDtcStatus(){
    return this->dtc_status_;
};
uint8_t DtcHeader::GetLength(){
    return this->lenght_;
};

void DtcHeader::SetDtcID(const uint16_t& value){
    this->dtc_id_=value;
};
void DtcHeader::SetMethodID(const uint8_t& value){
    this->dtc_status_=value;
};
void DtcHeader::SetLength(const uint8_t& value){
    this->lenght_=value+0x04;
};

}  // dtc
}  // mw
}  // simba
