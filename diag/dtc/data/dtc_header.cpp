#include "diag/dtc/data/dtc_header.hpp"

namespace simba{
namespace diag{
namespace dtc{

namespace{
    static constexpr uint8_t hdrSize=0x04;
}


DtcHeader::DtcHeader(const uint16_t &dtc_id,const uint8_t &dtc_status)
            :dtc_id_(dtc_id),dtc_status_(dtc_status){
                this->lenght_=hdrSize;
}

DtcHeader::DtcHeader():dtc_id_(0),dtc_status_(0){
    this->lenght_=hdrSize;
}

uint16_t DtcHeader::GetDtcID(){
    return this->dtc_id_;
};
uint8_t DtcHeader::GetDtcStatus(){
    return this->dtc_status_;
};
uint8_t DtcHeader::GetLength(){
    return this->lenght_;
};

void DtcHeader::SetLength(const uint8_t& value){
    this->lenght_=value+hdrSize;
};

}  // dtc
}  // mw
}  // simba
