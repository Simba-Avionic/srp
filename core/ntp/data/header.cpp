/**
 * @file header.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "core/ntp/data/header.hpp"

namespace simba {
namespace ntp {

    void Header::SetT1(uint64_t value) {
        this->T1.Set(value);
    }
    void Header::SetT2(uint64_t value) {
        this->T2.Set(value);
    }
    void Header::SetT3(uint64_t value) {
        this->T3.Set(value);
    }
    void Header::SetT4(uint64_t value) {
        this->T4.Set(value);
    }
    void Header::SetID(uint8_t ID) {
        this->ID.Set(ID);
    }
    Header::Header(uint64_t T1, uint64_t T2, uint64_t T3, uint64_t T4, uint8_t ID): T1(T1), T2(T2), T3(T3), T4(T4), ID(ID) {
        this->SetData();
    }
    void Header::SetData() {
        this->AddData(&T1);
        this->AddData(&T2);
        this->AddData(&T3);
        this->AddData(&T4);
        this->AddData(&ID);
    }
}
}