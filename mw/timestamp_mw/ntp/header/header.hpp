/**
 * @file header.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef MW_TIMESTAMP_MW_NTP_HEADER_HEADER_HPP_
#define MW_TIMESTAMP_MW_NTP_HEADER_HEADER_HPP_
#include "communication-core/network-data/network_data_structure.h"
#include "communication-core/network-data/network_data_type.h"

namespace srp {
namespace tinyNTP {

enum NTPMode_t {
  RESERVER = 0,
  SYMETRIC_ACTIVE = 1,
  SYMETRIC_PASIVE = 2,
  CLIENT = 3,
  SERVER = 4,
  BROADCAST = 5,
  CONTROL = 6,
  PRIVATE = 7
};

class NtpHeader : public com::core::network::NetworkDataStructure {
 private:
    com::core::network::uint8_t settings;
    com::core::network::int64_t t0;
    com::core::network::int64_t t1;
    com::core::network::int64_t t2;
    com::core::network::int64_t t3;

 public:
    NtpHeader();
    /**
     * @brief Construct a new Ntp Header object
     * 
     * @param stratum (3MSB) of settings
     * @param mode  (3LSB) of settings
     */
    void SetSettings(const uint8_t stratum, const NTPMode_t mode);
    void SetT0Timestamp(const int64_t& data);
    void SetT1Timestamp(const int64_t& data);
    void SetT2Timestamp(const int64_t& data);
    void SetT3Timestamp(const int64_t& data);
    void SetData();
    uint8_t GetSettings() { return settings.Get(); }
    int64_t GetT0Timestamp() { return t0.Get(); }
    int64_t GetT1Timestamp() { return t1.Get(); }
    int64_t GetT2Timestamp() { return t2.Get(); }
    int64_t GetT3Timestamp() { return t3.Get(); }
};

}  // namespace tinyNTP
}  // namespace srp

#endif  // MW_TIMESTAMP_MW_NTP_HEADER_HEADER_HPP_
