/**
 * @file header.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CORE_NTP_DATA_HEADER_HPP_
#define CORE_NTP_DATA_HEADER_HPP_

#include "communication-core/network-data/network_data_structure.h"
#include "communication-core/network-data/network_data_type.h"

namespace simba {
namespace ntp {

class Header : public com::core::network::NetworkDataStructure {
 private:
    com::core::network::uint64_t T1;  // Time when the client sent the request
    com::core::network::uint64_t T2;  // Time when the server received the request
    com::core::network::uint64_t T3;  // Time when the server sent the response
    com::core::network::uint64_t T4;  // Time when the client received the server's response
    com::core::network::uint8_t ID;  // id is lat section of IP adress
    void SetData();
 public:
    uint16_t GetT1() { return this->T1.Get(); }
    uint16_t GetT2() { return this->T2.Get(); }
    uint16_t GetT3() { return this->T3.Get(); }
    uint16_t GetT4() { return this->T4.Get(); }
    uint8_t GetID() { return this->ID.Get(); }
    void SetT1(uint64_t value);
    void SetT2(uint64_t value);
    void SetT3(uint64_t value);
    void SetT4(uint64_t value);
    void SetID(uint8_t ID);
    void SetData();
    Header(uint64_t T1, uint64_t T2, uint64_t T3, uint64_t T4, uint8_t ID);
};
}
}


#endif  // CORE_NTP_DATA_HEADER_HPP_

