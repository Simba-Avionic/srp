/**
 * @file IDtcClient.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _MW_IDTC_CLIENT_HPP
#define _MW_IDTC_CLIENT_HPP

#include <string>

namespace simba{
namespace mw{
namespace dtc{

class IDtcClient{
public:
 virtual void Init(uint16_t &service_id)=0;
 virtual void RegisterError(uint16_t error_code,std::string error_detail)=0;
 virtual std::string GetErrorDetails(uint16_t error_id)=0;
 virtual uint16_t GetNumberOfErrors()=0;
 virtual uint16_t ManualResetError(uint16_t error_id)=0;
 virtual uint16_t AutoResetError(uint16_t error_id)=0;
};


} // dtc
} // mw
} // simba


#endif // _MW_DTC_CLIENT_HPP