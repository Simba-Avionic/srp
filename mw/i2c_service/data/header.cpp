/**
 * @file header.cpp
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mw/i2c_service/data/header.h"

namespace srp {
namespace i2c {

Header::Header(ACTION action, uint8_t address) {
    this->action = action;
    this->address = address;
    this->payload_size = 0;
    this->SetData();
}

void Header::SetData() {
    this->AddData(&action);
    this->AddData(&address);
    this->AddData(&payload_size);
}

}  // namespace i2c
}  // namespace srp
