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

#include <cstdint>
#include <vector>
#include <cstring>
#include <optional>

namespace simba {
namespace ntp {

class Header {
 private:
    uint64_t T1;  // Time when the client sent the request
    uint64_t T2;  // Time when the server received the request
    uint64_t T3;  // Time when the server sent the response
    uint64_t T4;  // Time when the client received the server's response
    uint8_t ID;  // id is lat section of IP adress

 public:
  uint64_t GetT1() { return this->T1; }
  uint64_t GetT2() { return this->T2; }
  uint64_t GetT3() { return this->T3; }
  uint64_t GetT4() { return this->T4; }
  uint8_t GetID() { return this->ID; }
  void SetT1(uint64_t value);
  void SetT2(uint64_t value);
  void SetT3(uint64_t value);
  void SetT4(uint64_t value);
  void SetID(uint8_t ID);
  std::vector<uint8_t> GetBuffor() {
    std::vector<uint8_t> buffer(4*sizeof(T1) + sizeof(ID));
    size_t offset = 0;
    std::memcpy(buffer.data() + offset, &T1, sizeof(T1));
    offset += sizeof(T1);
    std::memcpy(buffer.data() + offset, &T2, sizeof(T2));
    offset += sizeof(T2);
    std::memcpy(buffer.data() + offset, &T3, sizeof(T3));
    offset += sizeof(T3);
    std::memcpy(buffer.data() + offset, &T4, sizeof(T4));
    offset += sizeof(T4);
    std::memcpy(buffer.data() + offset, &ID, sizeof(ID));
    return buffer;
  }
  static std::optional<Header> GetFromBuffor(const std::vector<uint8_t>& buffor) {
     if (buffor.size() != sizeof(uint64_t) * 4 + sizeof(uint8_t)) {
       return std::nullopt;
     }
     size_t offset = 0;
     uint64_t T1, T2, T3, T4;
     uint8_t ID;
     memcpy(&T1, buffor.data() + offset, sizeof(T1));
     offset += sizeof(T1);
     memcpy(&T2, buffor.data() + offset, sizeof(T2));
     offset += sizeof(T2);
     memcpy(&T3, buffor.data() + offset, sizeof(T3));
     offset += sizeof(T3);
     memcpy(&T4, buffor.data() + offset, sizeof(T4));
     offset += sizeof(T4);
     memcpy(&ID, buffor.data() + offset, sizeof(ID));
     return Header(T1, T2, T3, T4, ID);
  }
  Header(uint64_t T1, uint64_t T2, uint64_t T3, uint64_t T4, uint8_t ID);
};
}
}


#endif  // CORE_NTP_DATA_HEADER_HPP_

