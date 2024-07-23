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
enum ACTION:uint8_t {
    kRead = 0x01,
    kWrite = 0x02,
    kWriteRead = 0x03,
    kRES = 0x04,
    kPageWrite = 0x05,
};

class Header : public com::core::network::NetworkDataStructure {
 private:
    com::core::network::uint8_t action;
    com::core::network::uint8_t address;
    com::core::network::uint8_t payload_size;
    void SetData();
 public:
    ACTION GetAction() { return static_cast<ACTION>(this->action.Get()); }
    uint8_t GetAddress() { return this->address.Get(); }
    uint8_t GetPayloadSize() { return this->payload_size.Get(); }
    void SetPaylaodSize(uint8_t payload_size) {
        this->payload_size.Set(payload_size);
        }
    void SetAction(ACTION action) { this->action.Set(static_cast<uint8_t>(action)); }
    Header(ACTION action, uint8_t address);
};

}  // namespace i2c
}  // namespace simba




#endif  // MW_I2C_SERVICE_DATA_HEADER_H_
