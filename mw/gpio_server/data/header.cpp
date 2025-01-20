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

namespace srp {
namespace gpio {

Header::Header(uint8_t actuator_id, uint8_t value, ACTION action) {
    this->pin_id = actuator_id;
    this->value = value;
    this->action = static_cast<uint8_t>(action);
    this->SetData();
}

void Header::SetValue(uint8_t value) {
    this->value.Set(value);
}
void Header::SetAction(ACTION action) {
    this->action.Set(static_cast<uint8_t>(action));
}
void  Header::SetPinID(uint8_t pin_id) {
    this->pin_id.Set(pin_id);
}

void Header::SetData() {
    this->AddData(&action);
    this->AddData(&pin_id);
    this->AddData(&value);
}

}  // namespace gpio
}  // namespace srp
