/**
 * @file DtcClient.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _MW_DTC_CLIENT_HPP
#define _MW_DTC_CLIENT_HPP

#include "mw/diag/dtc/client/IDtcClient.hpp"
#include "communication-core/sockets/ipc_socket.h"
#include "nlohmann/json.hpp"
#include "core/logger/Logger.h"

#include <string>

namespace simba{
namespace mw{
namespace dtc{

class DtcClient:IDtcClient{
private:
 uint16_t service_id;
 com::soc::IpcSocket sock_;
 nlohmann::json data;


public:
 DtcClient(){};
 void Init(uint16_t &service_id) override;
 void RegisterError(uint16_t error_code,std::string error_detail)override;
 std::string GetErrorDetails(uint16_t error_id)override ;
 uint16_t GetNumberOfErrors()override;
 uint16_t ManualResetError(uint16_t error_id) override;
 uint16_t AutoResetError(uint16_t error_id) override;

 void RxCallback(const std::string& ip, const std::uint16_t& port,const std::vector<std::uint8_t> data);

};


} // dtc
} // mw
} // simba


#endif // _MW_DTC_CLIENT_HPP