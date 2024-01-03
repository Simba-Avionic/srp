/**
 * @file dtc_header.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _SRP_DTC_HEADER_HPP_
#define _SRP_DTC_HEADER_HPP_

#include "communication-core/network-data/network_data_structure.h"

#include <vector>
#include <chrono>

namespace simba{
namespace diag{
namespace dtc{

class DtcHeader: public com::core::network::NetworkDataStructure{
 private:
    //numer błędu
    uint16_t dtc_id_;
    // flagi błędu
    uint8_t dtc_status_;
    uint8_t lenght_;
 public:
   DtcHeader(uint16_t &dtc_id,uint8_t &dtc_status);
   DtcHeader();

   uint16_t GetDtcID();
   uint8_t GetDtcStatus();
   uint8_t GetLength();

   void SetDtcID(const uint16_t& value);
   void SetDtcStatus(const uint8_t& value);

   /**
    * @brief Set the Length object
    * 
    * @param value  (set only lenght of payload )
    */
   void SetLength(const uint8_t& value);
};

}  // dtc
}  // mw
}  // simba


#endif // _SRP_DTC_HEADER_HPP_
