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

namespace simba {
namespace i2c {

Header::Header(ACTION action, uint8_t address, uint16_t service_id) {
    this->action = action;
    this->address = address;
    this->service_id = service_id;
    this->payload_size = 0;
    this->SetData();
}

void Header::SetData() {
    this->AddData(&action);
    this->AddData(&address);
    this->AddData(&service_id);
    this->AddData(&payload_size);
}

}  // namespace i2c
}  // namespace simba
