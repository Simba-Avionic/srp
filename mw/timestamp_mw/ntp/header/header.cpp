/**
 * @file header.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "mw/timestamp_mw/ntp/header/header.hpp"

namespace srp {
namespace tinyNTP {


NtpHeader::NtpHeader() {
    SetData();
}
void NtpHeader::SetSettings(const uint8_t stratum, const NTPMode_t mode) {
    const uint8_t val = (mode & 0x07) | ((stratum & 0x07) << 3);
    this->settings.Set(val);
}
void NtpHeader::SetT0Timestamp(const int64_t& data) {
    this->t0.Set(data);
}
void NtpHeader::SetT1Timestamp(const int64_t& data) {
    this->t1.Set(data);
}
void NtpHeader::SetT2Timestamp(const int64_t& data) {
    this->t2.Set(data);
}
void NtpHeader::SetT3Timestamp(const int64_t& data) {
    this->t3.Set(data);
}
void NtpHeader::SetData() {
    AddData(&settings);
    AddData(&t0);
    AddData(&t1);
    AddData(&t2);
    AddData(&t3);
}

}  // namespace tinyNTP
}  // namespace srp
