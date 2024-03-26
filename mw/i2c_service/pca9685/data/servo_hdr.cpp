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
    this->AddData(&actuator_id);
    this->AddData(&position);
    this->AddData(&mode);
}

ServoHdr::ServoHdr(uint8_t actuator_id, uint16_t position, smode_t mode) {
    this->actuator_id = actuator_id;
    this->position = position;
    this->mode = static_cast<uint8_t>(mode);
    this->SetData();
}

}  // namespace i2c
}  // namespace simba
