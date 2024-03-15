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

Header::Header(ACTION action, uint8_t address, uint8_t reg, uint8_t service_id) {
    this->action = action;
    this->address = address;
    this->reg = reg;
    this->service_id = service_id;
    this->SetData();
}

void Header::SetData() {
    this->AddData(&action);
    this->AddData(&address);
    this->AddData(&reg);
    this->AddData(&service_id);
}

}  // namespace i2c
}  // namespace simba
