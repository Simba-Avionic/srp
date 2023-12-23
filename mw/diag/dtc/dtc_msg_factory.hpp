/**
 * @file dtc_header_factory.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _MW_DTC_Msg_FACTORY_HPP_
#define _MW_DTC_Msg_FACTORY_HPP_

#include "mw/diag/dtc/dtc_header.hpp"
#include <vector>
#include <memory>

namespace simba{
namespace mw{
namespace dtc{

class DtcMsgFactory{
 private:
    
 public:
  /**
  * @brief  This function return ready bit stream to send
  * 
  * @param header 
  * @param payload 
  * @return std::vector<uint8_t> 
  */
  std::vector<uint8_t> GetBuffer(std::shared_ptr<DtcHeader> header,std::vector<uint8_t> payload);

  /**
   * @brief Creat header object from raw data
   * 
   * @param raw_data 
   * @return std::shared_ptr<DtcHeader> 
   */
  std::shared_ptr<DtcHeader> GetHeader(std::vector<uint8_t> raw_data);

  /**
   * @brief Get payload from raw data
   * 
   * @param raw_data 
   * @return std::vector<uint8_t> 
   */
  std::vector<uint8_t> GetPayload(std::vector<uint8_t> raw_data);




};

}  // dtc
}  // mw
}  // simba

#endif // _MW_DTC_HEADER_FACTORY_HPP_