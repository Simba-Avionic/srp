/**
 * @file I_dtc_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef DTC_CONTROLLER_DTC_I_DTC_CONTROLLER_HPP_
#define DTC_CONTROLLER_DTC_I_DTC_CONTROLLER_HPP_

#include "core/common/error_code.h"
#include "communication-core/sockets/socket_config.h"

#include <vector>

namespace simba{
namespace diag{
namespace dtc{

class IDtcController{
public:
 virtual void Init(std::shared_ptr<com::soc::SocketConfig> config)=0;

 virtual core::ErrorCode RegisterError(const uint16_t &dtc_error_id,const uint8_t &dtc_status,const std::vector<uint8_t> &payload)=0;

};

} // dtc
} // diag
} // simba


#endif // DTC_CONTROLLER_DTC_I_DTC_CONTROLLER_HPP_
