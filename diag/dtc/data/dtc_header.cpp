/**
 * @file dtc_header.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "diag/dtc/data/dtc_header.hpp"

namespace simba {
namespace diag {
namespace dtc {

namespace {
static constexpr uint8_t hdrSize = 0x04;
}

DtcHeader::DtcHeader(const uint16_t &dtc_id, const uint8_t &dtc_status)
            :dtc_id_(dtc_id), dtc_status_(dtc_status) {
                this->lenght_ = hdrSize;
                this->SetData();
    }
DtcHeader::DtcHeader():dtc_id_{0}, dtc_status_{0} {
    this->SetData();
};

uint16_t DtcHeader::GetDtcID() {
    return this->dtc_id_.Get();
}
uint8_t DtcHeader::GetDtcStatus() {
    return this->dtc_status_.Get();
}
uint8_t DtcHeader::GetLength() {
    return this->lenght_.Get();
}

void DtcHeader::SetLength(const uint8_t& value) {
    this->lenght_.Set(value+hdrSize);
}

}  // namespace dtc
}  // namespace diag
}  // namespace simba
