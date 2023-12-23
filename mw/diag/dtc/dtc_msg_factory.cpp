/**
 * @file dtc_msg_factory.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "mw/diag/dtc/dtc_msg_factory.hpp"

namespace simba{
namespace mw{
namespace dtc{

std::vector<uint8_t> DtcMsgFactory::GetBuffer(std::shared_ptr<DtcHeader> header,std::vector<uint8_t> payload){
    std::vector<std::uint8_t> res{header->GetBuffor()};
    std::copy(payload.begin(), payload.end(), std::back_inserter(res));
    return res;
}

std::shared_ptr<DtcHeader> DtcMsgFactory::GetHeader(std::vector<uint8_t> raw_data){
    auto header=std::make_shared<DtcHeader>();
    header->SetBuffor(raw_data);
    return header;
}

std::vector<uint8_t> DtcMsgFactory::GetPayload(std::vector<uint8_t> raw_data){
    std::vector<uint8_t> payload{};
    std::copy(raw_data.begin() + 0x10, raw_data.end(), std::back_inserter(payload));
    return payload;
}



}  // dtc
}  // mw
}  // simba
