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

#include "eeprom_hdr.hpp"

namespace simba {
namespace i2c {


void EepromHdr::SetData() {
    this->AddData(&client_id);
    this->AddData(&transmition_id);
    this->AddData(&operation);
    this->AddData(&address);
}

EepromHdr::EepromHdr(uint16_t client_id, uint8_t transmition_id, operation_t operation, uint8_t address) {
    this->client_id = client_id;
    this->transmition_id = transmition_id;
    this->operation = static_cast<uint8_t>(operation);
    this->address = address;
    this->SetData();
}

}  // namespace i2c
}  // namespace simba
