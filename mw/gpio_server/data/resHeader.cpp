/**
 * @file resHeader.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "resHeader.hpp"
#include <cstdint>

namespace simba {
namespace gpio {

ResHeader::ResHeader(uint16_t pin_id, uint8_t value){
    this->pin_id = pin_id;
    this->value = value;
    this->SetData();
}

void ResHeader::SetData() {
    this->AddData(&pin_id);
    this->AddData(&value);
}

}  // gpio
}  // simba
