/**
 * @file header.h
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_I2C_SERVICE_DATA_HEADER_H_
#define MW_I2C_SERVICE_DATA_HEADER_H_

#include <cstdint>

#include "communication-core/network-data/network_data_structure.h"
#include "communication-core/network-data/network_data_type.h"


namespace simba {
namespace i2c {

class Header : public com::core::network::NetworkDataStructure {
 private:
    com::core::network::uint8_t action;
    com::core::network::uint8_t address;
    com::core::network::uint8_t reg;
    com::core::network::uint8_t service_id;
    void SetData();
 public:
    enum ACTION {
        Read = 0x01,
        Write = 0x02,
        ReadWrite = 0x03
    };
    ACTION GetAction() { return static_cast<ACTION>(this->action.Get()); }
    uint8_t GetAddress() { return this->address.Get(); }
    uint8_t GetReg() { return this->reg.Get(); }
    uint8_t GetServiceId() { return this->service_id.Get(); }
    Header(ACTION action, uint8_t address, uint8_t reg, uint8_t service_id);
};

}  // namespace i2c
}  // namespace simba




#endif  // MW_I2C_SERVICE_DATA_HEADER_H_
