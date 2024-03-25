/**
 * @file servo_hdr.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "servo_hdr.hpp"

namespace simba {
namespace i2c {


void ServoHdr::SetData() {
    this->AddData(&service_id);
    this->AddData(&actuator_id);
    this->AddData(&position);
}

ServoHdr::ServoHdr(uint16_t service_id, uint8_t actuator_id, uint8_t position) {
    this->service_id = service_id;
    this->actuator_id = actuator_id;
    this->position = position;
    this->SetData();
}

}  // namespace i2c
}  // namespace simba
