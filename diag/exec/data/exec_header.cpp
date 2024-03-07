/**
 * @file exec_header.cpp
 * @author Wiktor Kawka (asinesak353@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "diag/exec/data/exec_header.hpp"

namespace simba {
namespace diag {
namespace exec {

ExecHeader::ExecHeader(const uint16_t &service_id,
    const uint16_t &time_stamp, uint8_t flags)
        :service_id_(service_id), time_stamp_(time_stamp), flags_(flags) {this->SetData();}
ExecHeader::ExecHeader():service_id_{0}, time_stamp_{0} {
    this->SetData();
}

uint16_t ExecHeader::GetServiceID() const {
    return this->service_id_.Get();
}
uint16_t ExecHeader::GetTimestamp() const {
    return this->time_stamp_.Get();
}
uint8_t ExecHeader::GetFlags() const {
    return this->flags_.Get();
}
void ExecHeader::IncrementTimeStamp() {
    this->time_stamp_.Set(this->time_stamp_.Get()+1);
}

}  // namespace exec
}  // namespace diag
}  // namespace simba
