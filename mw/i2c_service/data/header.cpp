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

#include "header.h"

namespace simba {
namespace i2c {

Header::Header(ACTION action, uint8_t address, uint16_t service_id, uint16_t transmissionID) {
    this->action = action;
    this->address = address;
    this->service_id = service_id;
    this->payload_size = 0;
    this->transmission_id = transmissionID;
    this->SetData();
}

void Header::SetData() {
    this->AddData(&action);
    this->AddData(&address);
    this->AddData(&service_id);
    this->AddData(&payload_size);
    this->AddData(&transmission_id);
}

}  // namespace i2c
}  // namespace simba
