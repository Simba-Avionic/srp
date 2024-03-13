/**
 * @file header.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "header.hpp"
#include <cstdint>

namespace simba {
namespace gpio {

Header::Header(uint16_t service_id, uint16_t pin_id, uint8_t value) {
    this->service_id = service_id;
    this->pin_id = pin_id;
    this->value = value;
    this->SetData();
}

void Header::SetData() {
    this->AddData(&service_id);
    this->AddData(&pin_id);
    this->AddData(&value);
}

}  // namespace gpio
}  // namespace simba
