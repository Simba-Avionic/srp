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

#include <chrono>
#include "communication-core/network-data/network_data_structure.h"

namespace simba{
namespace mw{
namespace dtc{

class DtcHeader: public com::core::network::NetworkDataStructure{
 private:
    uint16_t service_id_;
    uint16_t method_id_;
    uint16_t lenght_;
 public:
   DtcHeader(uint16_t &service_id,uint16_t &method_id);
   DtcHeader();

   uint16_t GetServiceID();
   uint16_t GetMethodID();
   uint16_t GetLength();

   void SetServiceID(const uint16_t& value);
   void SetMethodID(const uint16_t& value);

   /**
    * @brief Set the Length object
    * 
    * @param value  (set only lenght of payload )
    */
   void SetLength(const uint16_t& value);
};

}  // dtc
}  // mw
}  // simba


#endif // _SRP_DTC_HEADER_HPP_
