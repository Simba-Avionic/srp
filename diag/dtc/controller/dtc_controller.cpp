/**
 * @file dtc_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "diag/dtc/controller/dtc_controller.hpp"
#include "diag/dtc/data/dtc_header.hpp"


namespace simba{
namespace diag{
namespace dtc{

void DtcController::Init(com::soc::SocketConfig &config){
    this->sock_.Init(config);
};

core::ErrorCode DtcController::RegisterError(uint16_t &dtc_error_id,uint8_t &dtc_status,std::vector<uint8_t> &payload){
    DtcHeader hdr{dtc_error_id,dtc_status};
    std::vector<uint8_t> data=this->factory_.GetBuffer(std::make_shared<DtcHeader>(hdr),payload);
    return this->sock_.Transmit("0x0101",0,data);
}

} // dtc
} // diag
} // simba
